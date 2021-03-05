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
#include "shaders/PickingShader.hpp"
#include "shaders/SimpleColorShader.hpp"
#include "textures/PickingTexture.hpp"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

static long long get_current_time_ms() {
	return GetTickCount();
}

// MESA_GLSL_VERSION_OVERRIDE=330 MESA_GL_VERSION_OVERRIDE=3.3 ./ogl

class Main : public CallbackInterface {
private:
	LightTechnique				lightShader_;
	PickingShader				pickingShader_;
	SimpleColorShader			simpleColorShader_;
	std::unique_ptr< Camera >	pCamera_;
	DirectionLight				dirLight_;
	Mesh						mesh_;
	PickingTexture				pickingTexture_;
	struct {
		bool IsPressed;
		int  x;
		int y;
	} leftMouseButton_;
	Vector3f					worldPos_[2];
	PersProjInfo				persProjInfo_;
	
public:
	Main() {
		dirLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
		dirLight_.AmbientIntensity = 1.0f;
		dirLight_.DiffuseIntensity = 0.08f;
		dirLight_.Direction = Vector3f(1.0f, -1.0f, 0.0f);
		
		leftMouseButton_.IsPressed = false;
		worldPos_[0] = Vector3f(-10.0f, 0.0f, 5.0f);
		worldPos_[1] = Vector3f(10.0f, 0.0f, 5.0f);

		persProjInfo_.FOV = 60.0f;
		persProjInfo_.Height = WINDOW_HEIGHT;
		persProjInfo_.Width = WINDOW_WIDTH;
		persProjInfo_.zNear = 1.0f;
		persProjInfo_.zFar = 100.0f;
	}
public: // == Main ==
    bool init(){
		Vector3f pos(0.0f, 5.0f, -22.0f);
		Vector3f target(0.0f, -0.2f, 1.0f);
		Vector3f up(0.0f, 1.0f, 0.0f);

		pCamera_ = std::make_unique< Camera >(WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up);
	
		if (!lightShader_.init()) {
			std::cerr << "Failed to initialize light shader" << std::endl;
			return false;
		}

		lightShader_.enable();
		lightShader_.setColorTextureUnit( COLOR_TEXTURE_UNIT_INDEX );
		lightShader_.setDirectionLight(dirLight_);

		if (not pickingTexture_.init(WINDOW_WIDTH, WINDOW_HEIGHT))
			return false;

		if (!pickingShader_.init())
			return false;

		if (!simpleColorShader_.init())
			return false;

		return mesh_.loadMesh("../Content/spider.obj");
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
	void pickPhase() {
		Pipeline p;
		p.scale(0.1f, 0.1f, 0.1f);
		p.rotate(0.0f, 90.0f, 0.0f);
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);

		pickingTexture_.enableWriting();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pickingShader_.enable();

		for (size_t idx = 0; idx < 2; idx++) {
			p.worldPos(worldPos_[idx]);
			pickingShader_.setObjectIndex(idx);
			pickingShader_.setWVP(p.getWVPTransformation());
			mesh_.render(&pickingShader_);
		}

		pickingTexture_.disableWriting();
	}

	void renderPhase() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Pipeline p;
		p.scale(0.1f, 0.1f, 0.1f);
		p.rotate(0.0f, 90.0f, 0.0f);
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);

		if (leftMouseButton_.IsPressed) {
			PickingTexture::PixelInfo pixel = pickingTexture_.readPixel(leftMouseButton_.x, WINDOW_HEIGHT - leftMouseButton_.y - 1);

			if (pixel.PrimId != 0) {
				simpleColorShader_.enable();
				assert(pixel.ObjectId < ARRAY_SIZE_IN_ELEMENTS( worldPos_ ));
				p.worldPos(worldPos_[ (unsigned)pixel.ObjectId ] );
				simpleColorShader_.setWVP(p.getWVPTransformation());
				mesh_.render((unsigned)pixel.DrawId, (unsigned)pixel.PrimId - 1);
			}
		}

		lightShader_.enable();
		lightShader_.setEyeWorldPosition(pCamera_->getPos());

		for (size_t idx = 0; idx < 2; idx++) {
			p.worldPos(worldPos_[idx]);
			lightShader_.setWVP(p.getWVPTransformation());
			lightShader_.setWorldMatrix(p.getWorldTransformation());
			mesh_.render();
		}
	}

    virtual void renderSceneCB() override{
		pCamera_->onRender();
		pickPhase();
		renderPhase();
		
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
		case 'a':
			dirLight_.AmbientIntensity += 0.05f;
			break;
		case 's':
			dirLight_.AmbientIntensity -= 0.05f;
			break;
		case 'z':
			dirLight_.DiffuseIntensity += 0.05f;
			break;
		case 'x':
			dirLight_.DiffuseIntensity -= 0.05f;
			break;
        default:
            break;
        }
    }

    virtual void passiveMouseCB(int x, int y)override{
        pCamera_->onMouse( x, y );
    }

	virtual void mouseCallback(int btn, int state, int x, int y) override {
		if (btn == GLUT_LEFT_BUTTON) {
			leftMouseButton_.IsPressed = (state == GLUT_DOWN);
			leftMouseButton_.x = x;
			leftMouseButton_.y = y;
		}
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
