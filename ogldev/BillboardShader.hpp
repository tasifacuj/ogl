#pragma once
#include "math_3d.h"
#include "Technique.hpp"

class BillboardShader : public Technique{
    GLuint vpLocation_{};
    GLuint cameraPosLocation_{};
    GLuint colorMapLocation_{};
public:// == Technique ==
    virtual bool init() override;
public:// == BillboardShader ==
    void setVP( Matrix4f const& m );
    void setCameraPosition( Vector3f const& pos );
    void setColorTextureUnit( unsigned textureUnitId );
};
