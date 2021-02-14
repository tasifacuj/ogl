#pragma once

#include <GL/glew.h>

class ShadowMapFBO{
    GLuint  fbo_{ 0 };
    GLuint  shadowMap_{ 0 };
public: // = CTORs ==
    ShadowMapFBO() = default;
    ~ShadowMapFBO();
public:// == ShadowMapFBO ==
    bool init( unsigned w, unsigned h );
    void bindToWrite();
    void bindToRead( GLenum textureUnit );
};