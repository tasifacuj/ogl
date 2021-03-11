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
	float						scale_{ 0.0f };
	DirectionalLight			directionalLight_;
	BasicMesh					mesh1_;
	BasicMesh					mesh2_;
	BasicMesh					mesh3_;
	PersProjInfo				persProjInfo_;
public:
	Main() {
		directionalLight_.Color = Vector3f(1.0f, 1.0f, 1.0f);
		directionalLight_.AmbientIntensity = 0.25f;
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
		Vector3f pos(3.0f, 7.0f, -10.0f);
		Vector3f target(0.0f, -0.2f, 1.0f);
		Vector3f up(0.0f, 1.0f, 0.0f);

		pCamera_ = std::make_unique< Camera >(WINDOW_WIDTH, WINDOW_HEIGHT, pos, target, up);
		
		if (!lightShader_.init()) {
			std::cerr << "Failed to load basic light shader\n";
			return false;
		}

		lightShader_.enable();
		lightShader_.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		lightShader_.SetDirectionalLight( directionalLight_ );
		lightShader_.SetMatSpecularIntensity(0.0f);
		lightShader_.SetMatSpecularPower(0);

		if (!mesh1_.loadMesh("../Content/phoenix_ugv.md2"))
			return false;

		if (!mesh2_.loadMesh("../Content/jeep.obj"))
			return false;

		if (!mesh3_.loadMesh("../Content/hheli.obj"))
			return false;

		return true;
    }

    void run(){
        GLUTBackendRun( this );
    }
public:// == CallbackInterface ==
	

    virtual void renderSceneCB() override{
		scale_ += 0.01f;
		pCamera_->onRender();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightShader_.SetEyeWorldPos(pCamera_->getPos());
		
		Pipeline p;
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.rotate(0.0f, scale_, 0.0f);
		p.setPerspectiveProjection(persProjInfo_);

		p.scale(0.1f, 0.1f, 0.1f);
		p.worldPos(-6.0f, -2.0f, 10.0f);
		lightShader_.SetWVP(p.getWVPTransformation());
		lightShader_.SetWorldMatrix(p.getWorldTransformation());
		mesh1_.render();

		p.scale(0.01f, 0.01f, 0.01f);
		p.worldPos(6.0f, -2.0f, 10.0f);
		lightShader_.SetWVP(p.getWVPTransformation());
		lightShader_.SetWorldMatrix(p.getWorldTransformation());
		mesh2_.render();

		p.scale(0.04f, 0.04f, 0.04f);
		p.worldPos(0.0f, 6.0f, 10.0f);
		lightShader_.SetWVP(p.getWVPTransformation());
		lightShader_.SetWorldMatrix(p.getWorldTransformation());
		mesh3_.render();

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

    if( !GLUTBackendCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "nice app" ) )
        return -1;

    Main app;

    if( !app.init() )
        return -2;

    app.run();
    return 0;
}
