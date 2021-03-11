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
#include "GlutBackEnd.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"


#include "EngineCommon.hpp"
#include "shaders/PickingShader.hpp"
#include "shaders/SimpleColorShader.hpp"
#include "textures/PickingTexture.hpp"

#include "ogl_keys.hpp"

#define WINDOW_WIDTH  1680
#define WINDOW_HEIGHT 1050

static long long get_current_time_ms() {
	return GetTickCount();
}

// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface {
private:
	LightingShader				lightShader_;
	std::unique_ptr< Camera >	pCamera_;
	DirectionalLight			dirLight_;
	Mesh						mesh_;
	PersProjInfo				persProjInfo_;
	float						tesselationLevel_;
	bool						isWireFrame_;
public:
	Main() {
		dirLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
		dirLight_.AmbientIntensity = 1.0f;
		dirLight_.DiffuseIntensity = 0.9f;
		dirLight_.Direction = Vector3f(0.0f, 0.0f, 1.0f);

		persProjInfo_.FOV = 60.0f;
		persProjInfo_.Height = WINDOW_HEIGHT;
		persProjInfo_.Width = WINDOW_WIDTH;
		persProjInfo_.zNear = 1.0f;
		persProjInfo_.zFar = 100.0f;

		tesselationLevel_ = 5.0f;
		isWireFrame_ = false;
	}
public: // == Main ==
    bool init(){
		Vector3f pos(0.0f, 1.0f, -5.0f);
		Vector3f target(0.0f, -0.2f, 1.0f);
		Vector3f up(0.0f, 1.0f, 0.0f);
		pCamera_ = std::make_unique< Camera >(WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up);

		if (!lightShader_.init()) {
			std::cerr << "Failed to load light shader" << std::endl;
			return false;
		}

		GLint maxPatchVerices = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVerices);
		std::cout << "MAx supported patch vertices = " << maxPatchVerices << std::endl;
		
		
		lightShader_.enable();
		lightShader_.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);// say that we use GL_TEXTURE0
		
		lightShader_.SetDirectionalLight(dirLight_);
		return mesh_.LoadMesh("../Content/monkey.obj");
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
	

    virtual void renderSceneCB() override{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pCamera_->onRender();

		Pipeline p;
		p.worldPos(-3.0f, 0.0f, 0.0f);
		p.scale(2.0f, 2.0f, 2.0f);
		p.rotate(-90.0f, 15.0f, 0.0f);
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);
		lightShader_.SetEyeWorldPos(pCamera_->getPos());

		lightShader_.SetVP(p.getViewProjectionTransformation());
		lightShader_.SetWorldMatrix(p.getWorldTransformation());
		lightShader_.SetTesselationLevel(tesselationLevel_);
		mesh_.Render(nullptr);

		p.worldPos(3.0f, 0.0f, 0.0f);
		p.rotate(-90.0f, -15.0f, 0.0f);
		lightShader_.SetVP(p.getViewProjectionTransformation());
		lightShader_.SetWorldMatrix(p.getWorldTransformation());
		lightShader_.SetTesselationLevel(1.0f);
		mesh_.Render( nullptr );
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
		case OGLDEV_KEY_PLUS:
			tesselationLevel_ += 1.0f;
			break;
		case OGLDEV_KEY_MINUS:
			if(tesselationLevel_ > 2.0f )
				tesselationLevel_ -= 1.0f;
			break;
		case 'z':
			isWireFrame_ = !isWireFrame_;

			if (isWireFrame_)
				glPolygonMode(GL_FRONT, GL_LINE);
			else
				glPolygonMode(GL_FRONT, GL_FILL);
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
