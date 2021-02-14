#pragma once

#include "Technique.hpp"
#include "math_3d.h"

class ShadowMapTechnique : public Technique{
    GLuint  WVPLocation_{ 0 };
    GLuint  textureLocation_{ 0 };
public: // == Technique ==
    virtual bool init() override;
public: // == ShadowMapTechnique ==
    void setWVP( Matrix4f const & wvp );
    void setTextureUnit( unsigned textureUnitId );
};
