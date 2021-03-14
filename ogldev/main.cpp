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
		
	}
public: // == Main ==
    bool init(){
		
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
	

    virtual void renderSceneCB() override{
		

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
