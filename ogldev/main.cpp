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

#include "EngineCommon.hpp"
#include "BillboardList.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024


// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface{
public: // == Main ==
    bool init(){
        dirLight_.AmbientIntensity = 0.2f;
        dirLight_.DiffuseIntensity = 0.8f;
        dirLight_.Color = Vector3f( 1.0f, 1.0f, 1.0f );
        dirLight_.Direction = Vector3f( 1.0f, 0.0f, 0.0f );

        ppi_.FOV = 60.0f;
        ppi_.Height = WINDOW_HEIGHT;
        ppi_.Width = WINDOW_WIDTH;
        ppi_.zNear = 1.0f;
        ppi_.zFar = 100.0f;

        Vector3f Pos(0.0f, 1.0f, -1.0f);
        Vector3f Target(0.0f, -0.5f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        pCamera_ = std::make_unique< Camera >( WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

// main shader
        if( !lightEffectShader_.init() ){
            std::cerr << "Failed to setup light technique" << std::endl;
            return false;
        }

        lightEffectShader_.useProgram();
        lightEffectShader_.setDirectionLight( dirLight_ );
        lightEffectShader_.setColorTextureUnit( 0 );
        lightEffectShader_.setNormalMapTextureUnit( 2 );

        if( not ground_.loadMesh( "/home/tez/projects/ogl/Content/quad.obj" ) )
            return false;

        if( not bbl_.init( "/home/tez/projects/ogl/Content/monster_hellknight.png" ) ){
            std::cerr << "Failed to initialize billboard" << std::endl;
            return false;
        }

        pTexture_ = std::make_unique< Texture >( GL_TEXTURE_2D, "/home/tez/projects/ogl/Content/bricks.jpg" );

        if( !pTexture_->load() ){
            std::cerr << "Failed to initialize bricks texture" << std::endl;
            return false;
        }

        pTexture_->bind( COLOR_TEXTURE_UNIT );
        pNormalMap_ = std::make_unique< Texture >( GL_TEXTURE_2D, "/home/tez/projects/ogl/Content/normal_map.jpg" );

        if( not pNormalMap_->load() )
            return false;

        return true;
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
    virtual void renderSceneCB() override{
        pCamera_->onRender();

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        lightEffectShader_.enable();
        pTexture_->bind( COLOR_TEXTURE_UNIT );
        pNormalMap_->bind( NORMAL_TEXTURE_UNIT );

        Pipeline p;
        p.scale( 20.0f, 20.0f, 1.0f );
        p.rotate( 90.0f, 0.0f, 0.0f );
        p.worldPos( 0.0f, 0.0f, 3.0f );
        p.setCamera( pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp() );
        p.setPerspectiveProjection( ppi_ );


        lightEffectShader_.setWVP( p.getWVPTransformation() );
        lightEffectShader_.setWorldMatrix( p.getWorldTransformation() );
        ground_.render();
        bbl_.render( p.get );

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
    DirectionLight              dirLight_;
    Mesh                        ground_;
    std::unique_ptr< Texture >  pTexture_;
    std::unique_ptr< Texture >  pNormalMap_;

    PersProjInfo                ppi_;
    BillboardList               bbl_;
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
