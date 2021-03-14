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

#include "import/BasicMesh.hpp"

#include "ogl_keys.hpp"

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
	LightingShader				lightShader_;
	std::unique_ptr< Camera >	pCamera_;
	float						scale_{ 0.0f };
	DirectionalLight			directionalLight_;
	BasicMesh					mesh_;
	PersProjInfo				persProjInfo_;
	Vector3f					positions_[NUM_INSTANCES];
	float						velocity_[NUM_INSTANCES];
public:
	Main() {
		directionalLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
		directionalLight_.AmbientIntensity = 0.55f;
		directionalLight_.DiffuseIntensity = 0.9f;
		directionalLight_.Direction = Vector3f(1.0f, 0.0f, 0.0f);

		persProjInfo_.FOV = 60.0f;
		persProjInfo_.Height = WINDOW_HEIGHT;
		persProjInfo_.Width = WINDOW_WIDTH;
		persProjInfo_.zNear = 1.0f;
		persProjInfo_.zFar = 100.0f;
	}
public: // == Main ==
    bool init(){
		Vector3f pos(7.0f, 3.0f, 0.0f);
		Vector3f target(0.0f, -0.2f, 1.0f);
		Vector3f up(0.0f, 1.0f, 0.0f);
		pCamera_ = std::make_unique< Camera >(WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up);
		lightShader_.setShaderPath("shaders/instance_light_vs.glsl", "shaders/instance_light_fs.glsl");

		if (!lightShader_.init()) {
			std::cerr << "Failed to setup light shader\n";
			return false;
		}

		lightShader_.enable();
		lightShader_.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		lightShader_.SetDirectionalLight(directionalLight_);
		lightShader_.SetMatSpecularIntensity(0.0f);
		lightShader_.SetMatSpecularPower(0.0f);
		lightShader_.set4Colors(0, Vector4f(1.0f, 0.5f, 0.5f, 0.0f));
		lightShader_.set4Colors(1, Vector4f(0.5f, 1.0f, 1.0f, 0.0f));
		lightShader_.set4Colors(2, Vector4f(1.0f, 0.5f, 1.0f, 0.0f));
		lightShader_.set4Colors(3, Vector4f(1.0f, 1.0f, 1.0f, 0.0f));

		if (!mesh_.loadMesh("../Content/spider.obj"))
			return false;

		claculatePositions();
		return true;
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
	

    virtual void renderSceneCB() override{
		scale_ += 0.005f;
		pCamera_->onRender();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightShader_.enable();
		lightShader_.SetEyeWorldPos(pCamera_->getPos());

		Pipeline p;
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);
		p.rotate(0.0f, 90.0f, 0.0f);
		p.scale(0.005f, 0.005f, 0.005f);

		Matrix4f wvpMatrices[NUM_INSTANCES];
		Matrix4f worldMatrices[NUM_INSTANCES];

		for (size_t idx = 0; idx < NUM_INSTANCES; idx++) {
			Vector3f pos(positions_[idx]);
			pos.y += sin(scale_) * velocity_[idx];
			p.worldPos(pos);
			wvpMatrices[idx] = p.getWVPTransformation().transpose();
			worldMatrices[idx] = p.getWorldTransformation().transpose();
		}

		mesh_.render(NUM_INSTANCES, wvpMatrices, worldMatrices);

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
	void claculatePositions() {
		for (int i = 0; i < NUM_ROWS; i++) {
			for (int j = 0; j < NUM_COLS; j++) {
				int idx = i * NUM_COLS + j;
				positions_[idx].x = float(j);
				positions_[idx].y = rand_float() * 5.0f;
				positions_[idx].z = float(i);
				velocity_[idx] = rand_float();

				if (i & 1)
					velocity_[idx] *= -1.0f;
			}
		}
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
