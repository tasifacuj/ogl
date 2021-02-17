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

#include "SkyBox.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024


// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface{
public: // == Main ==
    bool init(){
        dirLight_.AmbientIntensity = 0.2f;
        dirLight_.DiffuseIntensity = 0.8f;
        dirLight_.Color = Vector3f( 1.0f, 1.0f, 1.0f );
        dirLight_.Direction = Vector3f( 1.0f, -1.0f, 0.0f );

        ppi_.FOV = 60.0f;
        ppi_.Height = WINDOW_HEIGHT;
        ppi_.Width = WINDOW_WIDTH;
        ppi_.zNear = 1.0f;
        ppi_.zFar = 100.0f;

        Vector3f Pos(0.0f, 1.0f, -20.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        pCamera_ = std::make_unique< Camera >( WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

// main shader
        if( !lightEffectShader_.init() ){
            std::cerr << "Failed to setup light technique" << std::endl;
            return false;
        }

        lightEffectShader_.useProgram();
        lightEffectShader_.setDirectionLight( dirLight_ );
        lightEffectShader_.setTextureUnit( 0 );

        if( not tankMesh_.loadMesh( "/home/tez/projects/ogl/Content/phoenix_ugv.md2" ) )
            return false;

        pSkyBox_ = std::make_unique< SkyBox >( *pCamera_, ppi_ );

        if( not pSkyBox_->init( "/home/tez/projects/ogl/Content"
            , "sp3right.jpg"
            , "sp3left.jpg"
            , "sp3top.jpg"
            , "sp3bot.jpg"
            , "sp3front.jpg"
            , "sp3back.jpg" ) ){
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

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        lightEffectShader_.enable();
        Pipeline p;
        p.scale( 0.1f, 0.1f, 0.1f );
        p.rotate( 0.0f, scale_, 0.0f );
        p.worldPos( 0.0f, -1.0f, 1.0f );
        p.setCamera( pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp() );
        p.setPerspectiveProjection( ppi_ );

        lightEffectShader_.setWVP( p.getWVPTransformation() );
        lightEffectShader_.setWorldMatrix( p.getWorldTransformation() );
        tankMesh_.render();
        pSkyBox_->render();

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
    LightTechnique              lightEffectShader_;
    std::unique_ptr< Camera >   pCamera_;
    float                       scale_{};
    DirectionLight              dirLight_;
    Mesh                        tankMesh_;
    std::unique_ptr< SkyBox >   pSkyBox_;
    PersProjInfo                ppi_;
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
