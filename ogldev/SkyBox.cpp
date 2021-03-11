#include "SkyBox.hpp"
#include "Pipeline.hpp"
#include "Utils.hpp"

#include <iostream>


SkyBox::SkyBox(const Camera &c, const PersProjInfo &ppi)
: camera_( c )
, persProjInfo_( ppi ){}


bool SkyBox::init(const std::string &Directory
    , const std::string &PosXFilename
    , const std::string &NegXFilename
    , const std::string &PosYFilename
    , const std::string &NegYFilename
    , const std::string &PosZFilename
    , const std::string &NegZFilename){
    if( not skyboxShader_.init() ){
        std::cerr << "Failed to initialize skybox shader" << std::endl;
        return false;
    }

    skyboxShader_.useProgram();
    skyboxShader_.setTextureUnit( 0 );
    pCubemapTexture_ = std::make_unique< CubeMapTexture >( Directory, PosXFilename, NegXFilename, PosYFilename, NegYFilename, PosZFilename, NegZFilename );

    if( not pCubemapTexture_->load() )
        return false;

    return mesh_.LoadMesh( "/home/tez/projects/ogl/Content/sphere.obj" );
}

void SkyBox::render(){
    skyboxShader_.useProgram();

    GLint oldCullFaceMode;
    glGetIntegerv( GL_CULL_FACE_MODE, &oldCullFaceMode );
    GLint oldDepthFuncMode;
    glGetIntegerv( GL_DEPTH_FUNC, &oldDepthFuncMode );

    glCullFace( GL_FRONT );
    glDepthFunc( GL_LEQUAL );

    Pipeline p;
    p.scale( 20.0f, 20.0f, 20.0f );
    p.rotate( 0.0f, 0.0f, 0.0f );
    Vector3f const& cameraPos = camera_.getPos();
    p.worldPos( cameraPos.x, cameraPos.y, cameraPos.z );
    p.setCamera( cameraPos, camera_.getTarget(), camera_.getUp() );
    p.setPerspectiveProjection( persProjInfo_.FOV, persProjInfo_.Width, persProjInfo_.Height, persProjInfo_.zNear, persProjInfo_.zFar );;
    skyboxShader_.setWVP( p.getWVPTransformation() );
    pCubemapTexture_->bind( GL_TEXTURE0 );
    mesh_.Render( nullptr );

    glCullFace( oldCullFaceMode );
    glDepthFunc( oldDepthFuncMode );
}
