cmake_minimum_required(VERSION 3.10)
project( ogl )

set( SOURCES main.cpp
    Pipeline.cpp 
    math_3d.cpp 
    Camera.cpp 
    Texture.cpp 
    image_loader/stb_image.cpp 
    GLUTBackend.cpp 
    Technique.cpp 
    LightTechnique.cpp 
    Mesh.cpp
    ShadowMapFBO.cpp
    ShadowMapTechnique.cpp
    CubeMapTexture.cpp
    SkyboxShader.cpp
    SkyBox.cpp )

set( HEADERS
    math_3d.h 
    Pipeline.hpp 
    Camera.hpp 
    Texture.hpp
    image_loader/stb_image.h 
    GlutBackEnd.hpp 
    CallbackInterface.hpp 
    Technique.hpp 
    LightTechnique.hpp 
    Utils.hpp 
    Mesh.hpp
    ShadowMapFBO.hpp
    ShadowMapTechnique.hpp
    CubeMapTexture.hpp
    SkyboxShader.hpp
    SkyBox.hpp
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ogl  ${SOURCES} )

target_link_libraries( ogl glut GL GLEW assimp Magick++-6.Q16 MagickCore-6.Q16 )

include_directories( ogl /usr/include/ImageMagick-6  /usr/include/ImageMagick-6/Magick++ /usr/include/x86_64-linux-gnu/ImageMagick-6/ )
