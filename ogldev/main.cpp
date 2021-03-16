#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstring>
#include <cmath>
#include <cassert>
#include <iostream>
#include <memory>
#include <ctime>

#include "math_3d.h"
#include "Pipeline.hpp"
#include "Camera.hpp"
#include "Texture.hpp"

#include "shaders/LightingShader.hpp"
#include "shaders/DSGeometryPassShader.hpp"

#include "GlutBackEnd.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"
#include "EngineCommon.hpp"
#include "import/BasicMesh.hpp"
#include "ogl_keys.hpp"
#include "buffers/GBuffer.hpp"


static const int WINDOW_WIDTH = 1680;
static const int WINDOW_HEIGHT = 1050;
static const int NUM_ROWS = 50;
static const int NUM_COLS = 20;
static const int NUM_INSTANCES = NUM_ROWS * NUM_COLS;

static long long get_current_time_ms() {
	return GetTickCount();
}

// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface {
	DSGeometryPassShader		dsGeometryPassShader_;
	std::unique_ptr< Camera >	pCamera_;
	float						scale_{ 0.0f };
	BasicMesh					mesh_;
	PersProjInfo				persProjInfo_;
	GBuffer						gBuffer_;
public:
	Main() {
        persProjInfo_.FOV = 60.0f;
        persProjInfo_.Width = WINDOW_WIDTH;
        persProjInfo_.Height = WINDOW_HEIGHT;
        persProjInfo_.zNear = 1.0f;
        persProjInfo_.zFar = 100.0f;
	}
public: // == Main ==
    bool init(){
        if (!gBuffer_.init(WINDOW_WIDTH, WINDOW_HEIGHT))
            return false;

        pCamera_ = std::make_unique< Camera >(WINDOW_WIDTH, WINDOW_HEIGHT);

        if (!dsGeometryPassShader_.init()) {
            std::cerr << "Failed to setup DSGeometryPassShader\n";
            return false;
        }

        dsGeometryPassShader_.enable();
        dsGeometryPassShader_.setColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

        if (!mesh_.loadMesh("../Content/phoenix_ugv.md2"))
            return false;


        return true;
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
	

    virtual void renderSceneCB() override{
        scale_ += 0.05f;
        pCamera_->onRender();
        geometryPass();
        lightPass();
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
		case 27:
        case 'q':
            glutLeaveMainLoop();
            break;
		
        default:
			pCamera_->onKeyboard(key);
            break;
        }
    }

    virtual void passiveMouseCB(int x, int y)override{
        pCamera_->onMouse( x, y );
    }

	virtual void mouseCallback(int /*btn*/, int /*state*/, int /*x*/, int /*y*/) override {}
private:
    void geometryPass() {
        dsGeometryPassShader_.enable();
        gBuffer_.bindForWriting();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Pipeline p;
        p.scale(0.1f, 0.1f, 0.1f);
        p.rotate(0.0f, scale_, 0.0f);
        p.worldPos(-0.8f, -1.0f, 12.0f);
        p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
        p.setPerspectiveProjection(persProjInfo_);
        dsGeometryPassShader_.setWVP(p.getWVPTransformation());
        dsGeometryPassShader_.setWorldMatrix(p.getWorldTransformation());
        mesh_.render();
    }
    
    void lightPass() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBuffer_.bindForReading();
        GLint halfWidth = (GLint)(WINDOW_WIDTH / 2.0f);
        GLint halfHeight = (GLint)(WINDOW_HEIGHT / 2.0f);
        
        gBuffer_.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        gBuffer_.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, halfHeight, halfWidth, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        gBuffer_.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, halfWidth, halfHeight, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        gBuffer_.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, halfWidth, 0, WINDOW_WIDTH, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
};

int main( int argc, char** argv ){
    GLUTBackendInit( argc, argv );

    if( !GLUTBackendCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "nice app" ) )
        return -1;

	srand(time(nullptr));
    Main app;

    if( !app.init() )
        return -2;

    app.run();
    return 0;
}
