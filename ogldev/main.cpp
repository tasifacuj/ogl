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

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024


// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface{
public: // == Main ==
    bool init(){
        spotLight_.AmbientIntensity = 0.0f;
        spotLight_.DiffuseIntensity = 0.9f;
        spotLight_.Color = Vector3f( 1.0f, 1.0f, 1.0f );
        spotLight_.Attenuation.Linear = 0.01f;
        spotLight_.Position = Vector3f( -20.0f, 20.0f, 5.0f );
        spotLight_.Direction = Vector3f( 1.0f, -1.0f, 0.0f );
        spotLight_.CutOff = 20.0f;

        if( not shadowMapFBO_.init( WINDOW_WIDTH, WINDOW_HEIGHT ) )
            return false;

        pCamera_ = std::make_unique< Camera >( WINDOW_WIDTH, WINDOW_HEIGHT );
        plightEffect_ = std::make_unique< LightTechnique >();

        if( !plightEffect_->init() ){
            std::cerr << "Failed to setup light technique" << std::endl;
            return false;
        }

        p_shadowMapTech_ = std::make_unique< ShadowMapTechnique >();

        if( not p_shadowMapTech_->init() ){
            std::cerr << "Failed to setup shadow map tech" << std::endl;
            return false;
        }

        p_shadowMapTech_->enable();

        p_quad_ = std::make_unique< Mesh >();

        if( not p_quad_->loadMesh( "/home/tez/projects/ogldev/ogl/Content/quad.obj" ) ){
            std::cerr << "Failed to load mesh quad.obj" << std::endl;
            return false;
        }

        pMesh_ = std::make_unique< Mesh >();

        if( not pMesh_->loadMesh( "/home/tez/projects/ogldev/ogl/Content/phoenix_ugv.md2" ) ){
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
        scale_ += 0.1f;

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
        case 'a':
            dirLight_.AmbientIntensity += 0.05f;
            break;
        case 's':
            dirLight_.AmbientIntensity -= 0.05f;
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
        shadowMapFBO_.bindToWrite();
        glClear( GL_DEPTH_BUFFER_BIT );

        Pipeline p;
        p.scale( 0.2f, 0.2f, 0.2f );
        p.rotate( 0.0f, scale_, 0.0f );
        p.worldPos( 0.0f, 0.0f, 5.0f );
        p.setCamera( spotLight_.Position, spotLight_.Direction, Vector3f( 0.0f, 1.0f, 0.0f ) );
        p.setPerspectiveProjection( 60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f );
        p_shadowMapTech_->setWVP( p.getWVPTrans() );
        pMesh_->render();

        glBindBuffer( GL_FRAMEBUFFER, 0 );
    }

    void renderPath(){
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        p_shadowMapTech_->setTextureUnit( 0 );
        shadowMapFBO_.bindToRead( GL_TEXTURE0 );

        Pipeline p;
        p.scale( 5.0f, 5.0f, 5.0f );
        p.worldPos( 0.0f, 0.0f, 10.0f );
        p.setCamera( pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp() );
        p.setPerspectiveProjection( 60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f );
        p_shadowMapTech_->setWVP( p.getWVPTrans() );
        p_quad_->render();
    }
private:
    GLuint                              vbo_{};
    GLuint                              ibo_{};
    std::unique_ptr< LightTechnique >   plightEffect_;
    std::unique_ptr< ShadowMapTechnique > p_shadowMapTech_;
    std::unique_ptr< Mesh >             pMesh_;
    std::unique_ptr< Camera >           pCamera_;
    float                               scale_{};
    DirectionLight                      dirLight_;
    std::unique_ptr< Mesh >             p_quad_;
    ShadowMapFBO                        shadowMapFBO_;
    SpotLight                           spotLight_;
};

int main( int argc, char** argv ){
    GLUTBackendInit( argc, argv );

    if( !GLUTBackendCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 32, true, "nice app" ) )
        return -1;

    Main app;

    if( !app.init() )
        return -2;

    app.run();
    return 0;
}
