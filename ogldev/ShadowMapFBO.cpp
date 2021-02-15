#include "ShadowMapFBO.hpp"

#include <iostream>

ShadowMapFBO::~ShadowMapFBO(){
    if( fbo_ != 0 )
        glDeleteFramebuffers( 1, &fbo_ );

    if( shadowMap_ != 0 )
        glDeleteFramebuffers( 1, &shadowMap_ );
}

bool ShadowMapFBO::init( unsigned w, unsigned h ){
    // Создаем FBO
    glGenFramebuffers(1, &fbo_);

    // Создаем буфер глубины
    glGenTextures(1, &shadowMap_);
    glBindTexture(GL_TEXTURE_2D, shadowMap_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap_, 0);

    // Отключаем запись в буфер цвета
    glDrawBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE){
        printf("FB error, status: 0x%x\n", Status);
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
