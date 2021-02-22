#include "ShadowMapTechnique.hpp"

static constexpr const char* pVS = R"||(
#version 330 core

layout ( location = 0 ) in vec3 Position;
layout ( location = 1 ) in vec2 TexCoord;
layout ( location = 2 ) in vec3 Normal;

uniform mat4 gWVP;

out vec2 TexCoordOut;

void main(){
    gl_Position = gWVP * vec4( Position, 1.0 );
    TexCoordOut = TexCoord;
}
)||";

static constexpr const char* pFS = R"(
#version 330 core

in vec2 TexCoordOut;

uniform sampler2D gShadowMap;

out vec4 FragColor;

void main(){
    float depth = texture( gShadowMap, TexCoordOut ).x;
    depth = 1.0 - ( 1.0 - depth ) * 25.0;
    FragColor = vec4( depth );
}
)";

bool ShadowMapTechnique::init(){
    if( not Technique::init() )
        return false;

    if( not addShader( GL_VERTEX_SHADER, pVS ) )
        return false;

    if( not addShader( GL_FRAGMENT_SHADER, pFS ) )
        return false;

    if( not finalize() )
        return false;

    WVPLocation_ = getUniformLocation( "gWVP" );
    textureLocation_ = getUniformLocation( "gShadowMap" );

    if( INVALID_UNIFORM_LOCATION == WVPLocation_ || INVALID_UNIFORM_LOCATION == textureLocation_ )
        return false;

    return true;
}

void ShadowMapTechnique::setWVP( Matrix4f const& wvp ){
    glUniformMatrix4fv( WVPLocation_, 1, GL_TRUE, ( const GLfloat* )wvp.m );
}

void ShadowMapTechnique::setTextureUnit( unsigned textureUnitId ){
    glUniform1i( textureLocation_, textureUnitId );
}
