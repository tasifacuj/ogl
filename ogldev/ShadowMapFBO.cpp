#include "ShadowMapFBO.hpp"

#include <iostream>

ShadowMapFBO::~ShadowMapFBO(){
    if( fbo_ != 0 )
        glDeleteFramebuffers( 1, &fbo_ );

    if( shadowMap_ != 0 )
        glDeleteFramebuffers( 1, &shadowMap_ );
}

bool ShadowMapFBO::init( unsigned w, unsigned h ){
    glGenFramebuffers( 1, &fbo_ );

    glGenTextures( 1, &shadowMap_ );
    glBindTexture( GL_TEXTURE_2D, shadowMap_ );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo_ );// draw into fbo
    glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap_, 0 );// with help of shadow map texture

    // turn off color render
    glDrawBuffer( GL_NONE );
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

    if( status != GL_FRAMEBUFFER_COMPLETE ){
        std::cerr << "FB error, status = " << status << std::endl;
        return false;
    }

    return true;
}

void ShadowMapFBO::bindToWrite(){
    glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo_ );
}

void ShadowMapFBO::bindToRead( GLenum textureUnit ){
    glActiveTexture( textureUnit );
    glBindTexture( GL_TEXTURE_2D, shadowMap_ );
}