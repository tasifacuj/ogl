#pragma once

#include "Camera.hpp"
#include "SkyboxShader.hpp"
#include "CubeMapTexture.hpp"
#include "Mesh.hpp"

class SkyBox{
    SkyboxShader                    skyboxShader_;
    Camera const&                   camera_;
    std::unique_ptr<CubeMapTexture> pCubemapTexture_;
    Mesh                            mesh_;
    PersProjInfo                    persProjInfo_;

public: // == CTOR ==
    SkyBox( Camera const& c, PersProjInfo const& ppi );
public: // == SkyBox ==
    bool init( const std::string& Directory,
              const std::string& PosXFilename,
              const std::string& NegXFilename,
              const std::string& PosYFilename,
              const std::string& NegYFilename,
              const std::string& PosZFilename,
              const std::string& NegZFilename );
    void render();
};
