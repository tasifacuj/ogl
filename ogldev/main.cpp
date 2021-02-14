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

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024


// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface{
public: // == Main ==
    bool init(){
        dirLight_.Color = Vector3f( 1.0f, 1.0f, 1.0f );
        dirLight_.AmbientIntensity = 0.5f;
        dirLight_.DiffuseIntensity = 0.0f;
        dirLight_.Direction = Vector3f( 1.0f, 0.0f, 0.0f );

        Vector3f pos( -10.0f, 4.0f, 0.0f );
        Vector3f target( 1.0f, 0.0f, 1.0f );
//        Vector3f pos( 0.0f, 0.0f, -3.0f );
//        Vector3f target( 0.0f, 0.0f, -1.0f );

        Vector3f up( 0.0f, 1.0f, 0.0f );

        pCamera_ = std::make_unique< Camera >( WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up );
        plightEffect_ = std::make_unique< LightTechnique >();

        if( !plightEffect_->init() )
            return false;

         plightEffect_->enable();
         plightEffect_->setTextureUnit( 0 );
         pMesh_ = std::make_unique< Mesh >();
         return pMesh_->loadMesh( "/home/tez/projects/ogldev/ogl/Content/phoenix_ugv.md2" );
//        return pMesh_->loadMesh( "/home/tez/projects/ogldev/Content/box.obj" );

    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
    virtual void renderSceneCB() override{
        pCamera_->onRender();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        scale_ += 0.1f;


        Pipeline p;
        p.scale( 0.1f, 0.1f, 0.1f );
        p.rotate( 0.f, scale_, 0.f );
        p.worldPos( 0.0f, 0.0f, 10.0f );
        p.setCamera( pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp() );
        p.setPerspectiveProjection( 60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f );
        plightEffect_->setWVP( p.getWVPTrans() );

        const Matrix4f&  wt = p.getWorldTrans();
        plightEffect_->setWorldMatrix( wt );
        plightEffect_->setDirectionLight( dirLight_ );
        plightEffect_->setEyeWorldPosition( pCamera_->getPos() );
        plightEffect_->setMatSpecularIntensity( 0.0f );
        plightEffect_->setMatSpecularPower( 0 );

        pMesh_->render();
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
    void calculateNormals( const unsigned* indices, unsigned indSize, Vertex* pVertices, unsigned vertexCount ){
        for( unsigned idx = 0; idx < indSize; idx += 3 ){
            unsigned idx0 = indices[ idx ];
            unsigned idx1 = indices[ idx + 1 ];
            unsigned idx2 = indices[ idx + 2 ];

            Vector3f v1 = pVertices[ idx1 ].Pos - pVertices[ idx0 ].Pos;
            Vector3f v2 = pVertices[ idx2 ].Pos - pVertices[ idx0 ].Pos;
            Vector3f normal = v1.cross( v2 );
            normal.normalize();

            pVertices[ idx0 ].Normal += normal;
            pVertices[ idx1 ].Normal += normal;
            pVertices[ idx2 ].Normal += normal;
        }

        for( size_t idx = 0; idx < vertexCount; idx++ )
            pVertices[ idx ].Normal.normalize();
    }

    void createVertexBuffer( const unsigned* pIndices, unsigned indexCount ){
        Vertex vertices[] = {
           Vertex(Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) ),
           Vertex(Vector3f(0.0f, -1.0f, -1.15475), Vector2f(0.5f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f)),
           Vertex(Vector3f(1.0f, -1.0f, 0.5773f),  Vector2f(1.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f)),
           Vertex(Vector3f(0.0f, 1.0f, 0.0f),      Vector2f(0.5f, 1.0f), Vector3f(0.0f, 0.0f, 0.0f))
        };

        unsigned vertexCount = ARRAY_SIZE_IN_ELEMENTS( vertices );
        calculateNormals( pIndices, indexCount, vertices, vertexCount );

        glGenBuffers( 1, &vbo_ );
        glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    }

    void createIndexBuffer( const unsigned* indices, unsigned sizeInBytes ){
        glGenBuffers( 1, &ibo_ );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo_ );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, indices, GL_STATIC_DRAW );
    }
private:
    GLuint                              vbo_{};
    GLuint                              ibo_{};
    std::unique_ptr< LightTechnique >   plightEffect_;
    std::unique_ptr< Mesh >             pMesh_;
    std::unique_ptr< Camera >           pCamera_;
    float                               scale_{};
    DirectionLight                      dirLight_;
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
