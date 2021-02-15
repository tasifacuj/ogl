#include "LightTechnique.hpp"
#include "Utils.hpp"

#include <iostream>
#include <cstring>

constexpr const char* pVS = R"||(
#version 330

layout ( location = 0 ) in vec3 Position;
layout ( location = 1 ) in vec2 TexCoord;
layout ( location = 2 ) in vec3 Normal;

uniform mat4 gWVP;
uniform mat4 gWorld;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main(){
    gl_Position = gWVP * vec4( Position, 1.0 );
    TexCoord0 = TexCoord;
    Normal0  = ( gWorld * vec4( Normal, 0.0 ) ).xyz;
    WorldPos0 = ( gWorld * vec4( Position, 1.0 ) ).xyz;
}
)||";

constexpr const char* pFS = R"||(
#version 330

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

out vec4 FragColor;

struct BaseLight{
    vec3    Color;
    float   AmbientIntensity;
    float   DiffuseIntensity;
};

struct DirectionalLight{
    BaseLight   Base;
    vec3        Direction;
};

struct Attenuation{
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight{
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};

struct SpotLight{
    PointLight  Base;
    vec3        Direction;
    float       CutOff;
};

uniform int                 gNumPointLights;
uniform int                 gNumSpotLights;
uniform DirectionalLight    gDirectionalLight;
uniform PointLight          gPointLights[ MAX_POINT_LIGHTS ];
uniform SpotLight           gSpotLights[ MAX_SPOT_LIGHTS ];

uniform sampler2D           gSampler; // GL_TEXTURE0

uniform float               gSpecularPower;
uniform vec3                gEyeWorldPos;
uniform float               gMatSpecularIntensity;


vec4 calc_light_internal( BaseLight light, vec3 light_dir, vec3 normal ){
    vec4 ambient_color = vec4( light.Color, 1.0 ) * light.AmbientIntensity;
    float diffuse_factor = dot( normal, -light_dir );

    vec4 diffuse_color  = vec4( 0, 0, 0, 0 );
    vec4 specular_color = vec4( 0, 0, 0, 0 );

    if( diffuse_factor > 0 ){
        diffuse_color = vec4( light.Color, 1.0 ) * light.DiffuseIntensity * diffuse_factor;

        vec3 vertex_2_eye = normalize( gEyeWorldPos - WorldPos0 );
        vec3 light_reflected = normalize( reflect( light_dir, normal ) );
        float specular_factor = dot( vertex_2_eye, light_reflected );
        specular_factor = pow( specular_factor, gSpecularPower );

        if( specular_factor > 0 )
            specular_color = vec4( light.Color, 1.0 ) * gMatSpecularIntensity * specular_factor;

    }

    return (ambient_color + diffuse_color + specular_color);
}

vec4 calc_dir_light( vec3 normal ){
    return calc_light_internal( gDirectionalLight.Base, gDirectionalLight.Direction, normal );
}

vec4 calc_point_light( PointLight l, vec3 normal ){
    vec3 light_dir = WorldPos0 - l.Position;
    float distance = length( light_dir );
    light_dir = normalize( light_dir );

    vec4 color = calc_light_internal( l.Base, light_dir, normal );
    float attenuation =l.Atten.Constant + l.Atten.Linear * distance + l.Atten.Exp * distance * distance;

    return color / attenuation;
}

vec4 calc_spot_light( SpotLight l, vec3 normal ){
    vec3 light_2_pixel  = normalize( WorldPos0 - l.Base.Position );
    float spot_factor = dot( light_2_pixel, l.Direction );

    if( spot_factor > l.CutOff ){
        vec4 color = calc_point_light( l.Base, normal );
        return color * ( 1.0 - ( 1.0 - spot_factor ) * 1.0 / ( 1.0 - l.CutOff ) );
    }else{
        return vec4( 0.0, 0.0, 0.0, 0.0 );
    }
}

void main(){
    vec3 normal = normalize( Normal0 );
    vec4 total_light = calc_dir_light( normal );

    for( int i = 0; i < gNumPointLights; i++ ){
        total_light += calc_point_light( gPointLights[ i ], normal );
    }

    for( int i = 0; i < gNumSpotLights; i++ ){
        total_light += calc_spot_light( gSpotLights[ i ], normal );
    }

    FragColor = texture2D( gSampler, TexCoord0.xy ) * total_light;
}
)||";


static const int INVALID_LOCATION = 0xFFFFFFFF;

bool LightTechnique::init(){
    if( !Technique::init() )
        return false;

    std::cout << "00000\n";


    if( !addShader( GL_VERTEX_SHADER, pVS ) )
        return false;

    std::cout << "1111\n";

    if( !addShader( GL_FRAGMENT_SHADER, pFS ) )
        return false;

     std::cout << "22222\n";

    if( !finalize() )
        return false;


    std::cout << "33333"
                 "\n";
    wVPLoacation_ = getUniformLocation( "gWVP" );
    worldMatrixLocation_ = getUniformLocation( "gWorld" );
    samplerLocation_ = getUniformLocation( "gSampler" );

    eyeWorldPosition_ = getUniformLocation( "gEyeWorldPos" );
    matSpecularIntensityLocation_ = getUniformLocation( "gMatSpecularIntensity" );
    matSpecularPowerLocation_ = getUniformLocation( "gSpecularPower" );
// dir light
    dirLightLocation_.Color = getUniformLocation( "gDirectionalLight.Base.Color" );
    dirLightLocation_.AmbientIntensity = getUniformLocation( "gDirectionalLight.Base.AmbientIntensity" );
    dirLightLocation_.Direction = getUniformLocation( "gDirectionalLight.Direction" );
    dirLightLocation_.DiffuseIntensity = getUniformLocation( "gDirectionalLight.Base.DiffuseIntensity" );

// point light
    numPointLightsLocation_ = getUniformLocation( "gNumPointLights" );

    for( int idx = 0; idx < ARRAY_SIZE_IN_ELEMENTS( pointLightsLocation_ ); idx++ ){
        char name[ 128 ];
        memset( name, 0, sizeof( name ) );

        snprintf( name, sizeof( name ), "gPointLights[%d].Base.Color", idx );
        pointLightsLocation_[ idx ].Color = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gPointLights[%d].Base.AmbientIntensity", idx );
        pointLightsLocation_[ idx ].AmbientIntensity = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gPointLights[%d].Position", idx );
        pointLightsLocation_[ idx ].Position = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gPointLights[%d].Base.DiffuseIntensity", idx );
        pointLightsLocation_[ idx ].DiffuseIntensity = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gPointLights[%d].Atten.Constant", idx );
        pointLightsLocation_[ idx ].Atten.Constant = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gPointLights[%d].Atten.Linear", idx );
        pointLightsLocation_[ idx ].Atten.Linear = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gPointLights[%d].Atten.Exp", idx );
        pointLightsLocation_[ idx ].Atten.Exp = getUniformLocation( name );

        if( pointLightsLocation_[ idx ].Color == INVALID_LOCATION
        || pointLightsLocation_[ idx ].AmbientIntensity == INVALID_LOCATION
        || pointLightsLocation_[ idx ].Position == INVALID_LOCATION
        || pointLightsLocation_[ idx ].DiffuseIntensity == INVALID_LOCATION
        || pointLightsLocation_[ idx ].Atten.Constant == INVALID_LOCATION
        || pointLightsLocation_[ idx ].Atten.Linear == INVALID_LOCATION
        || pointLightsLocation_[ idx ].Atten.Exp == INVALID_LOCATION
        ){
            std::cerr << "Invalid point light location" << std::endl;
            return false;
        }
    }

  // spot lights
    numSpotLightsLocation_ = getUniformLocation( "gNumSpotLights" );

    for( unsigned idx = 0; idx < ARRAY_SIZE_IN_ELEMENTS( spotLightsLocation_ ); idx++ ){
        char name[ 128 ];
        memset( name, 0, sizeof( name ) );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Base.Color", idx );
        spotLightsLocation_[ idx ].Color = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Base.AmbientIntensity", idx );
        spotLightsLocation_[ idx ].AmbientIntensity = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Position", idx );
        spotLightsLocation_[ idx ].Position = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Direction", idx );
        spotLightsLocation_[ idx ].Direction = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].CutOff", idx );
        spotLightsLocation_[ idx ].CutOff = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Base.DiffuseIntensity", idx );
        spotLightsLocation_[ idx ].DiffuseIntensity = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Atten.Constant", idx );
        spotLightsLocation_[ idx ].Atten.Constant = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Atten.Linear", idx );
        spotLightsLocation_[ idx ].Atten.Linear = getUniformLocation( name );

        snprintf( name, sizeof( name ), "gSpotLights[%d].Base.Atten.Exp", idx );
        spotLightsLocation_[ idx ].Atten.Exp = getUniformLocation( name );

        if( spotLightsLocation_[ idx ].Color == INVALID_LOCATION
            || spotLightsLocation_[ idx ].AmbientIntensity == INVALID_LOCATION
            || spotLightsLocation_[ idx ].Position == INVALID_LOCATION
            || spotLightsLocation_[ idx ].Direction == INVALID_LOCATION
            || spotLightsLocation_[ idx ].CutOff == INVALID_LOCATION
            || spotLightsLocation_[ idx ].DiffuseIntensity == INVALID_LOCATION
            || spotLightsLocation_[ idx ].Atten.Constant == INVALID_LOCATION
            || spotLightsLocation_[ idx ].Atten.Linear == INVALID_LOCATION
            || spotLightsLocation_[ idx ].Atten.Exp == INVALID_LOCATION
        ){
            std::cerr << "Invalid point light location" << std::endl;
            return false;
        }
    }

    if( wVPLoacation_ == INVALID_LOCATION
    || worldMatrixLocation_ == INVALID_LOCATION
    || samplerLocation_ == INVALID_LOCATION
    || dirLightLocation_.Color == INVALID_LOCATION
    || dirLightLocation_.AmbientIntensity == INVALID_LOCATION
    || dirLightLocation_.Direction == INVALID_LOCATION
    || dirLightLocation_.DiffuseIntensity == INVALID_LOCATION
    || eyeWorldPosition_ == INVALID_LOCATION
    || matSpecularIntensityLocation_ == INVALID_LOCATION
    || matSpecularPowerLocation_ == INVALID_LOCATION
    || numSpotLightsLocation_ == INVALID_LOCATION
    )
        return false;

    return true;
}

void LightTechnique::setWVP(const Matrix4f &wvp){
    glUniformMatrix4fv( wVPLoacation_, 1, GL_TRUE, ( const GLfloat* ) wvp.m );
}

void LightTechnique::setWorldMatrix(const Matrix4f &wm){
    glUniformMatrix4fv( worldMatrixLocation_, 1, GL_TRUE, ( const GLfloat* ) wm.m );
}

void LightTechnique::setTextureUnit(unsigned textureUnit){
    glUniform1i( samplerLocation_, textureUnit );
}

void LightTechnique::setDirectionLight(const DirectionLight &dl){
    glUniform3f( dirLightLocation_.Color, dl.Color.x, dl.Color.y, dl.Color.z );
    glUniform1f( dirLightLocation_.AmbientIntensity, dl.AmbientIntensity );
    Vector3f dir = dl.Direction;
    dir.normalize();
    glUniform3f( dirLightLocation_.Direction, dir.x, dir.y, dir.z );
    glUniform1f( dirLightLocation_.DiffuseIntensity, dl.DiffuseIntensity );
}

void LightTechnique::setEyeWorldPosition( Vector3f const& eyeWorldPos ){
    glUniform3f( eyeWorldPosition_, eyeWorldPos.x, eyeWorldPos.y, eyeWorldPos.z );
}

void LightTechnique::setMatSpecularIntensity(float intensity){
    glUniform1f( matSpecularIntensityLocation_, intensity );
}

void LightTechnique::setMatSpecularPower(float power){
    glUniform1f( matSpecularPowerLocation_, power );
}

void LightTechnique::setPointLights(unsigned numLights, const PointLight *pLights){
    glUniform1i( numPointLightsLocation_, numLights );

    for( unsigned idx = 0; idx < numLights; idx++ ){
        glUniform3f( pointLightsLocation_[ idx ].Color,             pLights[ idx ].Color.x, pLights[ idx ].Color.y, pLights[ idx ].Color.z );
        glUniform1f( pointLightsLocation_[ idx ].AmbientIntensity,  pLights[ idx ].AmbientIntensity );
        glUniform1f( pointLightsLocation_[ idx ].DiffuseIntensity,  pLights[ idx ].DiffuseIntensity );
        glUniform3f( pointLightsLocation_[ idx ].Position,          pLights[ idx ].Position.x, pLights[ idx ].Position.y, pLights[ idx ].Position.z );
        glUniform1f( pointLightsLocation_[ idx ].Atten.Constant,    pLights[ idx ].Attenuation.Constatnt );
        glUniform1f( pointLightsLocation_[ idx ].Atten.Linear,      pLights[ idx ].Attenuation.Linear );
        glUniform1f( pointLightsLocation_[ idx ].Atten.Exp,         pLights[ idx ].Attenuation.Exp );
    }
}

void LightTechnique::setSpotLigts(unsigned nLights, const SpotLight *pLights){
    glUniform1i( numSpotLightsLocation_, nLights );

    for( unsigned idx = 0; idx < nLights; idx++ ){
        glUniform3f( spotLightsLocation_[ idx ].Color, pLights[ idx ].Color.x, pLights[ idx ].Color.y, pLights[ idx ].Color.z  );
        glUniform1f( spotLightsLocation_[ idx ].AmbientIntensity, pLights[ idx ].AmbientIntensity );
        glUniform1f( spotLightsLocation_[ idx ].DiffuseIntensity, pLights[ idx ].DiffuseIntensity );
        glUniform3f( spotLightsLocation_[ idx ].Position, pLights[ idx ].Position.x, pLights[ idx ].Position.y, pLights[ idx ].Position.z );
        Vector3f dir = pLights[ idx ].Direction;
        dir.normalize();
        glUniform3f( spotLightsLocation_[ idx ].Direction, dir.x, dir.y, dir.z );
        glUniform1f( spotLightsLocation_[ idx ].CutOff, cosf( ToRadian( pLights[ idx ].CutOff ) ) );

        glUniform1f( spotLightsLocation_[ idx ].Atten.Constant,    pLights[ idx ].Attenuation.Constatnt );
        glUniform1f( spotLightsLocation_[ idx ].Atten.Linear,      pLights[ idx ].Attenuation.Linear );
        glUniform1f( spotLightsLocation_[ idx ].Atten.Exp,         pLights[ idx ].Attenuation.Exp );
    }
}