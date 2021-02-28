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
#include "LightTechnique.hpp"
#include "GlutBackEnd.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"


#include "EngineCommon.hpp"
#include "ParticleSystem.hpp"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

static long long get_current_time_ms() {
	return GetTickCount();
}

// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface {
private:
	long long					currentTimeMillis_{ 0 };
	LightTechnique				lightShader_;
	std::unique_ptr< Camera >	pCamera_;
	DirectionLight				dirLight_;
	Mesh						ground_;
	std::unique_ptr< Texture >	pTexture_;
	std::unique_ptr< Texture >	pNormalMap_;
	PersProjInfo				persProjInfo_;
	ParticleSystem				particleSystem_;
public:
	Main() {
		dirLight_.AmbientIntensity = 0.2f;
		dirLight_.DiffuseIntensity = 0.8f;
		dirLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
		dirLight_.Direction = Vector3f(1.0f, 0.0f, 0.0f);

		persProjInfo_.FOV = 60.0f;
		persProjInfo_.Height = WINDOW_HEIGHT;
		persProjInfo_.Width = WINDOW_WIDTH;
		persProjInfo_.zNear = 1.0f;
		persProjInfo_.zFar = 100.0f;

		currentTimeMillis_ = get_current_time_ms();
	}
public: // == Main ==
    bool init(){
		Vector3f pos(0.0f, 0.4f, -0.5f);
		Vector3f target(0.0f, 0.2f, 1.0f);
		Vector3f up(0.0f, 1.0f, 0.0f);

		pCamera_ = std::make_unique< Camera >(WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up);
	
		if (!lightShader_.init()) {
			std::cerr << "Failed to initialize light shader" << std::endl;
			return false;
		}

		lightShader_.enable();
		lightShader_.setDirectionLight(dirLight_);
		lightShader_.setColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);// 0 == GL_TEXTURE0, gColrMap = 0
		lightShader_.setNormalMapTextureUnit(NORMAL_TEXTURE_UNIT_INDEX);

		if (!ground_.loadMesh("../Content/quad.obj")) {
			std::cerr << "Failed to load ground" << std::endl;
			return false;
		}

		pTexture_ = std::make_unique< Texture >(GL_TEXTURE_2D, "../Content/bricks.jpg");

		if (!pTexture_->load()) {
			std::cerr << "Failed to load texture" << std::endl;
			return false;
		}

		pTexture_->bind(COLOR_TEXTURE_UNIT);

		pNormalMap_ = std::make_unique< Texture >(GL_TEXTURE_2D, "../Content/normal_map.jpg");

		if (!pNormalMap_->load()) {
			std::cerr << "Failed to load normal map" << std::endl;
			return false;
		}

		Vector3f particlePos(0.0f, 0.0f, 1.0f);
		return particleSystem_.init(particlePos);
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
    virtual void renderSceneCB() override{
		long long timeNow = get_current_time_ms();
		assert(timeNow >= currentTimeMillis_);
		unsigned deltaTMs = unsigned(timeNow - currentTimeMillis_);
		currentTimeMillis_ = timeNow;
		pCamera_->onRender();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightShader_.enable();
		pTexture_->bind(COLOR_TEXTURE_UNIT);
		pNormalMap_->bind(NORMAL_TEXTURE_UNIT);

		Pipeline p;
		p.scale(20.0f, 20.0f, 1.0f);
		p.rotate(90.0f, 0.0f, 0.0f);
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);

		lightShader_.setWVP(p.getWVPTransformation());
		lightShader_.setWorldMatrix(p.getWorldTransformation());

		ground_.render();
		particleSystem_.render(deltaTMs, p.getViewProjectionTransformation(), pCamera_->getPos());

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
