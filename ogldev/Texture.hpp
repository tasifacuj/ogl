#pragma once

#include <string>

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class Texture{
    std::string     fileName_;
    GLenum          m_textureTarget{};
    GLuint          m_textureObj{};
    Magick::Image* m_pImage;
    Magick::Blob m_blob;
public: // == Texture ==
    Texture( GLenum target, std :: string const &name );
    bool load();
    void bind( GLenum textureUnit );
};
