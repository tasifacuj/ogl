#include "CubeMapTexture.hpp"
#include "Utils.hpp"

#include <iostream>

static const GLenum types [ 6 ] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

CubeMapTexture::CubeMapTexture( const std::string &dir
    , const std::string &posXFilename
    , const std::string &negXFilename
    , const std::string &posYFilename
    , const std::string &negYFilename
    , const std::string &posZFilename
    , const std::string &negZFilename){
    std::string::const_iterator it = dir.end();
    it--;
    std::string baseDir = ( *it == '/' ) ? dir : dir + "/";
    filenames_[ 0 ] = baseDir + posXFilename;
    filenames_[ 1 ] = baseDir + negXFilename;
    filenames_[ 2 ] = baseDir + posYFilename;
    filenames_[ 3 ] = baseDir + negYFilename;
    filenames_[ 4 ] = baseDir + posZFilename;
    filenames_[ 5 ] = baseDir + negZFilename;
}

CubeMapTexture::~CubeMapTexture(){
    if( 0 != textureObj_ )
        glDeleteTextures( 1, &textureObj_ );
}

bool CubeMapTexture::load(){
    glGenTextures( 1, &textureObj_ );
    glBindTexture( GL_TEXTURE_CUBE_MAP, textureObj_ );

    for( unsigned idx = 0; idx < ARRAY_SIZE_IN_ELEMENTS( types ); idx++ ){
        Magick::Image image( filenames_[ idx ] );
        Magick::Blob blob;

        try{
            image.write( &blob, "RGBA" );
        }catch( std::exception const& ex ){
            std::cerr << "CubeMapTexture::load ex: " << ex.what() << std::endl;
            return false;
        }

        glTexImage2D(types[ idx ], 0, GL_RGB, image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

void CubeMapTexture::bind(GLenum textureUnitId){
    glActiveTexture( textureUnitId );
    glBindTexture( GL_TEXTURE_CUBE_MAP, textureObj_ );
}

