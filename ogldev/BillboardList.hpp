#pragma once

#include "Texture.hpp"
#include "BillboardShader.hpp"
#include "math_3d.h"

#include <memory>
#include <string>

class BillboardList{
    GLuint                     vbo_{};
    std::unique_ptr< Texture > pTexture_;
    BillboardShader            shader_;

public:
    ~BillboardList();
    bool init( std::string const& pathToTexture );
    void render( Matrix4f const& vp, Vector3f const& cameraPos );
private:
    void createPositionBuffer();
};
