#include "BillboardShader.hpp"

static constexpr const char* pVS = R"(
#version 330

layout( location = 0 ) in vec3 Position;

void main(){
    gl_Position = vec4( Position, 1.0 );
}
)";

static constexpr const char* pGS = R"(
#version 330

layout(points) in;
layout( triangle_strip, max_vertices = 4 ) out;

uniform mat4 gVP;
uniform vec3 gCameraPos;

out vec2 TexCoord;

void main(){
    vec3 pos = gl_in[ 0 ].gl_Position.xyz;// bottom in the middle
    vec3 toCamera = normalize( gCameraPos - pos );
    vec3 up = vec3( 0.0, 1.0, 0.0 );
    vec3 right = cross( toCamera, up );

    // bottom left
    pos -= ( right * 0.5 );
    gl_Position = gVP * vec4( pos, 1.0 );
    TexCoord = vec2( 0.0, 0.0 );
    EmitVertex();

    // top left
    pos.y += 1.0;
    gl_Position = gVP * vec4( pos, 1.0 );
    TexCoord = vec2( 0.0, 1.0 );
    EmitVertex();

    // bottom right
    pos.y -= 1.0;
    pos += right;
    gl_Position = gVP * vec4( pos, 1.0 );
    TexCoord = vec2( 1.0, 0.0 );
    EmitVertex();

    // top right
    pos.y += 1.0;
    gl_Position = gVP * vec4( pos, 1.0 );
    TexCoord = vec2( 1.0, 1.0 );
    EmitVertex();

    EndPrimitive();
}
)";


static constexpr const char* pFS = R"(
#version 330 core

uniform sampler2D gColorMap;

in vec2 TexCoord;
out vec4 FragColor;

void main(){
    FragColor = texture2D( gColorMap, TexCoord );

    if( FragColor.r == 0 && FragColor.g == 0 && FragColor.b == 0 )
        discard;
}

)";


bool BillboardShader::init(){
    if( not Technique::init() )
        return false;

    if( not addShader( GL_VERTEX_SHADER, pVS ) )
        return false;

    if( not addShader( GL_GEOMETRY_SHADER, pGS ) )
        return false;

    if( not addShader( GL_FRAGMENT_SHADER, pFS ) )
        return false;

    if( not finalize() )
        return false;


    vpLocation_ = getUniformLocation( "gVP" );
    cameraPosLocation_ = getUniformLocation( "gCameraPos" );
    colorMapLocation_ = getUniformLocation( "gColorMap" );

    if( INVALID_UNIFORM_LOCATION == vpLocation_ || cameraPosLocation_ == INVALID_UNIFORM_LOCATION || INVALID_UNIFORM_LOCATION == colorMapLocation_ )
        return false;

    return true;
}

void BillboardShader::setVP(const Matrix4f &m){
    glUniformMatrix4fv( vpLocation_, 1, GL_TRUE, ( const GLfloat* )m.m );
}

void BillboardShader::setCameraPosition(const Vector3f &pos){
    glUniform3f( cameraPosLocation_, pos.x, pos.y, pos.z );
}

void BillboardShader::setColorTextureUnit(unsigned textureUnitId){
    glUniform1i( colorMapLocation_, textureUnitId );
}
