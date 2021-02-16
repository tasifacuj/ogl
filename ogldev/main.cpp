#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstring>
#include <cmath>
#include <cassert>
#include <iostream>
#include <memory>

#include "math_3d.h"
#include "Pipeline.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "LightTechnique.hpp"
#include "GlutBackEnd.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"
#include "ShadowMapTechnique.hpp"
#include "ShadowMapFBO.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024


// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface{
public: // == Main ==
    bool init(){
        spotLight_.AmbientIntensity = 0.1f;
        spotLight_.DiffuseIntensity = 0.9f;
        spotLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
        spotLight_.Attenuation.Linear = 0.01f;
        spotLight_.Position  = Vector3f(-20.0, 20.0, 1.0f);
        spotLight_.Direction = Vector3f(1.0f, -1.0f, 0.0f);
        spotLight_.CutOff =  20.0f;

         Vector3f Pos(3.0f, 8.0f, -10.0f);
        Vector3f Target(0.0f, -0.2f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);
        if( not shadowMapFBO_.init( WINDOW_WIDTH, WINDOW_HEIGHT ) )
            return false;

        pCamera_ = std::make_unique< Camera >( WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

// main shader
        pLightEffectShader_ = std::make_unique< LightTechnique >();

        if( !pLightEffectShader_->init() ){
            std::cerr << "Failed to setup light technique" << std::endl;
            return false;
        }

        pLightEffectShader_->useProgram();
        pLightEffectShader_->setSpotLigts( 1, &spotLight_ );
        pLightEffectShader_->setTextureUnit( 0 );// use GL_TEXTURE0
        pLightEffectShader_->setShadowMapTextureUnit( 1 );// use GL_TEXTURE1

        pShadowMapShader_ = std::make_unique< ShadowMapTechnique >();

        if( not pShadowMapShader_->init() ){
            std::cerr << "Failed to setup shadow map tech" << std::endl;
            return false;
        }

        pQuad_ = std::make_unique< Mesh >();

        if( not pQuad_->loadMesh( "/home/tez/projects/ogl/Content/quad.obj" ) ){
            std::cerr << "Failed to load mesh quad.obj" << std::endl;
            return false;
        }

        pGroundTex_ = std::make_unique< Texture >( GL_TEXTURE_2D, "/home/tez/projects/ogl/Content/test.png" );

        if( not pGroundTex_->load() ){
            std::cerr << "Failed to load ground texture" << std::endl;
            return false;
        }

        pMesh_ = std::make_unique< Mesh >();

        if( not pMesh_->loadMesh( "/home/tez/projects/ogl/Content/phoenix_ugv.md2" ) ){
            std::cerr << "Failed to load mesh phoenix_ugv" << std::endl;
            return false;
        }

        return true;
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
    virtual void renderSceneCB() override{
        pCamera_->onRender();
        scale_ += 0.5f;

        shadowMapPass();
        renderPath();

        glutSwapBuffers();
    }

    virtual void idleCB()override{
        renderSceneCB();
    }

    virtual void specialKeyboardCB(int key, int, int)override{
        pCamera_->onKeyboard( key );
    }

    virtual void keyboardCB(unsigned char key, int, int)override{
        switch (key) {
        case 'q':
            glutLeaveMainLoop();
            break;

        default:
            break;
        }
    }

    virtual void passiveMouseCB(int x, int y)override{
        pCamera_->onMouse( x, y );
    }

private:
    void shadowMapPass(){
        shadowMapFBO_.bindToWrite();// turn on depth test framebuffer and attached texture, that calculates depth
        glClear(GL_DEPTH_BUFFER_BIT);
        pShadowMapShader_->useProgram();

         Pipeline p;
        p.scale(0.1f, 0.1f, 0.1f);
        p.rotate(0.0f, scale_, 0.0f);
        p.worldPos(0.0f, 0.0f, 5.0f);
        p.setCamera(spotLight_.Position, spotLight_.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p.setPerspectiveProjection(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
        pShadowMapShader_->setWVP(p.getWVPTransformation());
        pMesh_->render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void renderPath(){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pLightEffectShader_->enable();

        shadowMapFBO_.bindToRead(GL_TEXTURE1);

        Pipeline p;
        p.setPerspectiveProjection(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
        p.scale(10.0f, 10.0f, 10.0f);
        p.worldPos(0.0f, 0.0f, 1.0f);
        p.rotate(90.0f, 0.0f, 0.0f);
        p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());

        pLightEffectShader_->setWVP(p.getWVPTransformation());
        pLightEffectShader_->setWorldMatrix(p.getWorldTransformation());
        p.setCamera(spotLight_.Position, spotLight_.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        pLightEffectShader_->setLightWVP(p.getWVPTransformation());
        pLightEffectShader_->setEyeWorldPosition(pCamera_->getPos());
        pGroundTex_->bind(GL_TEXTURE0);
        pQuad_->render();

        p.scale(0.1f, 0.1f, 0.1f);
        p.rotate(0.0f, scale_, 0.0f);
        p.worldPos(0.0f, 5.0f, 3.0f);
        p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
        pLightEffectShader_->setWVP(p.getWVPTransformation());
        pLightEffectShader_->setWorldMatrix(p.getWorldTransformation());
        p.setCamera(spotLight_.Position, spotLight_.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        pLightEffectShader_->setLightWVP(p.getWVPTransformation());

        pMesh_->render();
    }
private:
    GLuint                              vbo_{};
    GLuint                              ibo_{};
    std::unique_ptr< LightTechnique >   pLightEffectShader_;
    std::unique_ptr< ShadowMapTechnique > pShadowMapShader_;
    std::unique_ptr< Mesh >             pMesh_;
    std::unique_ptr< Mesh >             pQuad_;
    std::unique_ptr< Camera >           pCamera_;
    float                               scale_{};
    ShadowMapFBO                        shadowMapFBO_;
    SpotLight                           spotLight_;
    std::unique_ptr< Texture >          pGroundTex_;
};

int main( int argc, char** argv ){
    GLUTBackendInit( argc, argv );

    if( !GLUTBackendCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "nice app" ) )
        return -1;

    Main app;

    if( !app.init() )
        return -2;

    app.run();
    return 0;
}
