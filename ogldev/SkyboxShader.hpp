#pragma once
#include "Technique.hpp"

#include "math_3d.h"

class SkyboxShader : public Technique{
    GLuint WVPLocation_{};
    GLuint textureLocation_{};
public:
    SkyboxShader() = default;
    virtual bool init() override;
    void setWVP( Matrix4f const& wvp );
    void setTextureUnit( unsigned textureUnitId ); // GL_TEXTURE0, GL_TEXTURE1
};
