#include "Texture.hpp"
#include "image_loader/stb_image.h"

#include <iostream>
#include <cassert>


Texture::Texture(GLenum target, const std::string &name)
: fileName_( name )
, m_textureTarget( target ){
    assert( m_textureTarget ==  GL_TEXTURE_2D );
}



bool Texture::load(){
    try {
        m_pImage = new Magick::Image(fileName_);
        m_pImage->write(&m_blob, "RGBA");
    }catch (Magick::Error& Error) {
        std::cout << "Error loading texture '" << fileName_ << "': " << Error.what() << std::endl;
        return false;
    }

    glGenTextures(1, &m_textureObj);
    glBindTexture(m_textureTarget, m_textureObj);
    glTexImage2D(m_textureTarget, 0, GL_RGB, m_pImage->columns(), m_pImage->rows(), -0.5, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return true;
}

void Texture::bind(GLenum textureUnit){
    glActiveTexture( textureUnit );
    glBindTexture( m_textureTarget, m_textureObj );
}
