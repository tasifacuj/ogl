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
#include "CallbackInterface.hpp"
#include "GlutBackEnd.hpp"

#include "shaders/T38SkinnedShader.hpp"
#include "import/SkinnedMesh.hpp"

#include "Utils.hpp"
#include "EngineCommon.hpp"

#include "ogl_keys.hpp"



static const int WindowWidth = 1680;
static const int WindowHeight = 1050;
static const int NUM_ROWS = 50;
static const int NUM_COLS = 20;
static const int NUM_INSTANCES = NUM_ROWS * NUM_COLS;

static long long get_current_time_ms() {
	return GetTickCount();
}

// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface {
	T38SkinnedShader			shader_;
	std::unique_ptr< Camera >	pCamera_;
	DirectionalLight			directionalLight_;
	SkinnedMesh					mesh_;
	Vector3f					position_;
	PersProjInfo				persProjInfo_;
	long long					m_frameTime;
	long long					m_startTime;
public:
	Main() {
		directionalLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
		directionalLight_.AmbientIntensity = 0.55f;
		directionalLight_.DiffuseIntensity = 0.9f;
		directionalLight_.Direction = Vector3f(1.0f, 0.0, 0.0);

		persProjInfo_.FOV = 60.0f;
		persProjInfo_.Height = WindowHeight;
		persProjInfo_.Width = WindowWidth;
		persProjInfo_.zNear = 1.0f;
		persProjInfo_.zFar = 100.0f;

		position_ = Vector3f(0.0f, 0.0f, 6.0f);
		m_frameTime = m_startTime = GetTickCount();
	}

	float GetRunningTime()
	{
		float RunningTime = (float)((double)GetTickCount() - (double)m_startTime) / 1000.0f;
		return RunningTime;
	}
public: // == Main ==
    bool init(){
		Vector3f pos(0.0f, 3.0f, -1.0f);
		Vector3f target(0.0f, 0.0f, 1.0f);
		Vector3f up(0.0f, 1.0f, 0.0f);
		pCamera_ = std::make_unique< Camera >(WindowWidth, WindowHeight, pos, target, up);

		if (!shader_.init())
			return false;

		shader_.enable();
		shader_.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		shader_.SetDirectionalLight(directionalLight_);
		shader_.SetMatSpecularIntensity(0.0f);
		shader_.SetMatSpecularPower(0.0f);

		if (!mesh_.LoadMesh("../Content/boblampclean.md5mesh")) {
			std::cerr << "Failed to load mesh" << std::endl;
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_.enable();
		std::vector< Matrix4f > transforms;
		float runningTime = GetRunningTime();
		mesh_.BoneTransform(runningTime, transforms);

		for (size_t idx = 0; idx < transforms.size(); idx++) {
			shader_.SetBoneTransform(idx, transforms[idx]);
		}

		shader_.SetEyeWorldPos(pCamera_->getPos());
		Pipeline p;
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);
		p.scale(0.1f, 0.1f, 0.1f);

		Vector3f pos(position_);
		p.worldPos(pos);
		p.rotate(270.0f, 180.0f, 0.0f);
		shader_.SetWVP(p.getWVPTransformation());
		shader_.SetWorldMatrix(p.getWorldTransformation());
		mesh_.Render();

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

};

int main( int argc, char** argv ){
    GLUTBackendInit( argc, argv );

    if( !GLUTBackendCreateWindow( WindowWidth, WindowHeight, 32, false, "nice app" ) )
        return -1;

	srand(time(nullptr));
    Main app;

    if( !app.init() )
        return -2;

    app.run();
    return 0;
}
