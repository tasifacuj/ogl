#include "SkyboxShader.hpp"
#include "Utils.hpp"

static constexpr const char* pVS = R"(
#version 330 core

layout ( location = 0 ) in vec3 Position;

uniform mat4 gWVP;

out vec3 TexCoord0;

void main(){
    vec4 WVPPos = gWVP * vec4( Position, 1.0 );
    gl_Position = WVPPos.xyww;
    TexCoord0 = Position;
}

)";

static constexpr const char* pFS = R"(
#version 330 core

in vec3 TexCoord0;

out vec4 FragColor;

uniform samplerCube gCubemapTexture;

void main(){
    FragColor = texture( gCubemapTexture, TexCoord0 );
}
)";


bool SkyboxShader::init(){
    if( not Technique::init() )
        return false;

    if( not addShader( GL_VERTEX_SHADER, pVS ) )
        return false;

    if( not addShader( GL_FRAGMENT_SHADER, pFS ) )
        return false;

    if( not finalize() )
        return false;

    WVPLocation_ = getUniformLocation( "gWVP" );
    textureLocation_ = getUniformLocation( "gCubemapTexture" );

    if( INVALID_UNIFORM_LOCATION == WVPLocation_ || INVALID_UNIFORM_LOCATION == textureLocation_ )
        return false;

    return true;
}

void SkyboxShader::setWVP(const Matrix4f &wvp){
    glUniformMatrix4fv( WVPLocation_, 1, GL_TRUE, ( const GLfloat* )wvp.m );
}

void SkyboxShader::setTextureUnit(unsigned textureUnitId){
    glUniform1i( textureLocation_, textureUnitId );
}
