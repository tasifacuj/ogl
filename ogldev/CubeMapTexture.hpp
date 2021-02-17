#pragma once

#include <string>
#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class CubeMapTexture{
    std::string filenames_[ 6 ];
    GLuint      textureObj_{};
public:
    CubeMapTexture( std::string const& dir
        , std::string const& posXFilename
        , std::string const& negXFilename
        , std::string const& posYFilename
        , std::string const& negYFilename
        , std::string const& posZFilename
        , std::string const& negZFilename
        );

    ~CubeMapTexture();
public:// == CubeMapTexture ==
    bool load();
    void bind( GLenum textureUnitId );
};
