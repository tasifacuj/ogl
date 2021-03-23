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


#include "shaders/DSGeometryPassShader.hpp"
#include "shaders/DSDirLightPassShader.hpp"
#include "shaders/DSPointLightShader.hpp"
#include "buffers/GBuffer.hpp"

#include "GlutBackEnd.hpp"
#include "Utils.hpp"
#include "EngineCommon.hpp"
#include "import/BasicMesh.hpp"
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
	DSGeometryPassShader		dsGeometryPassShader_;
	DSPointLightShader			dsPointLightShader_;
	DSDirLightPassShader		dsDirLightPassShader_;
	std::unique_ptr< Camera >	pCamera_;
	float						scale_{ 0.0f };
	SpotLight					spotLight_;
	DirectionalLight			dirLight_;
	PointLight					pointLights_[ 3 ];
	BasicMesh					box_;
	BasicMesh					sphere_;
	BasicMesh					quad_;
	PersProjInfo				persProjInfo_;
	GBuffer						gBuffer_;
	Vector3f					boxPositions_[5];
private:
	void setupLights() {
		spotLight_.AmbientIntensity = 0.0f;
		spotLight_.DiffuseIntensity = 0.9f;
		spotLight_.Color = COLOR_WHITE;
		spotLight_.Attenuation.Linear = 0.01f;
		spotLight_.Position = Vector3f(-20.0, 20.0, 5.0f);
		spotLight_.Direction = Vector3f(1.0f, -1.0f, 0.0f);
		spotLight_.CutOff = 20.0f;

		dirLight_.AmbientIntensity = 0.1f;
		dirLight_.Color = COLOR_CYAN;
		dirLight_.DiffuseIntensity = 0.5f;
		dirLight_.Direction = Vector3f(1.0f, 0.0f, 0.0f);

		pointLights_[0].DiffuseIntensity = 0.2f;
		pointLights_[0].Color = COLOR_GREEN;
		pointLights_[0].Position = Vector3f(0.0f, 1.5f, 5.0f);
		pointLights_[0].Attenuation.Constant = 0.0f;
		pointLights_[0].Attenuation.Linear = 0.0f;
		pointLights_[0].Attenuation.Exp = 0.3f;

		pointLights_[1].DiffuseIntensity = 0.2f;
		pointLights_[1].Color = COLOR_RED;
		pointLights_[1].Position = Vector3f(2.0f, 0.0f, 5.0f);
		pointLights_[1].Attenuation.Constant = 0.0f;
		pointLights_[1].Attenuation.Linear = 0.0f;
		pointLights_[1].Attenuation.Exp = 0.3f;

		pointLights_[2].DiffuseIntensity = 0.2f;
		pointLights_[2].Color = COLOR_BLUE;
		pointLights_[2].Position = Vector3f(0.0f, 0.0f, 3.0f);
		pointLights_[2].Attenuation.Constant = 0.0f;
		pointLights_[2].Attenuation.Linear = 0.0f;
		pointLights_[2].Attenuation.Exp = 0.3f;
	}

	void setupBoxPositions() {
		boxPositions_[0] = Vector3f(0.0f, 0.0f, 5.0f);
		boxPositions_[1] = Vector3f(6.0f, 1.0f, 10.0f);
		boxPositions_[2] = Vector3f(-5.0f, -1.0f, 12.0f);
		boxPositions_[3] = Vector3f(4.0f, 4.0f, 15.0f);
		boxPositions_[4] = Vector3f(-4.0f, 2.0f, 20.0f);
	}

	void geometryPass() {
		dsGeometryPassShader_.enable();
		gBuffer_.bindForWriting();
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		Pipeline p;
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);
		p.rotate(0.0f, scale_, 0.0f);

		for (size_t idx = 0; idx < ARRAY_SIZE_IN_ELEMENTS(boxPositions_); idx++) {
			p.worldPos(boxPositions_[idx]);
			dsGeometryPassShader_.setWVP(p.getWVPTransformation());
			dsGeometryPassShader_.setWorldMatrix(p.getWorldTransformation());
			box_.render();
		}

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

	void beginLightPass() {
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		gBuffer_.bindForReading();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	float CalcPointLightBSphere(PointLight const& light) {
		float maxChannel = std::max(std::max(light.Color.x, light.Color.y), light.Color.z);
		return (-light.Attenuation.Linear 
					+ std::sqrt(light.Attenuation.Linear * light.Attenuation.Linear 
						- 4 * light.Attenuation.Exp * (light.Attenuation.Exp - 256 * maxChannel * light.DiffuseIntensity)
					)
				)
				/
				(2 * light.Attenuation.Exp);
	}

	void pointLightPass() {
		dsPointLightShader_.enable();
		dsPointLightShader_.SetEyeWorldPos(pCamera_->getPos());

		Pipeline p;
		p.setCamera(pCamera_->getPos(), pCamera_->getTarget(), pCamera_->getUp());
		p.setPerspectiveProjection(persProjInfo_);

		for (size_t idx = 0; idx < ARRAY_SIZE_IN_ELEMENTS(pointLights_); idx++) {
			dsPointLightShader_.setPointLight(pointLights_[idx]);
			p.worldPos(pointLights_[idx].Position);
			float scale = CalcPointLightBSphere(pointLights_[idx]);
			p.scale(scale, scale, scale);
			dsPointLightShader_.SetWVP(p.getWVPTransformation());
			sphere_.render();
		}
	}

	void dirLightPass() {
		dsDirLightPassShader_.enable();
		dsDirLightPassShader_.SetEyeWorldPos(pCamera_->getPos());
		Matrix4f wvp;
		wvp.InitIdentity();
		dsDirLightPassShader_.SetWVP(wvp);
		quad_.render();
	}
	
public:
	Main() {
        persProjInfo_.FOV = 60.0f;
        persProjInfo_.Width = WindowWidth;
        persProjInfo_.Height = WindowHeight;
        persProjInfo_.zNear = 1.0f;
        persProjInfo_.zFar = 100.0f;

		setupLights();
		setupBoxPositions();
	}
public: // == Main ==
    bool init(){
		if (!gBuffer_.init(WindowWidth, WindowHeight))
			return false;

		pCamera_ = std::make_unique< Camera >(WindowWidth, WindowHeight);

		if (!dsGeometryPassShader_.init()) {
			std::cerr << "Failed to setup DSGeometryPass\n";
			return false;
		}

		dsGeometryPassShader_.enable();
		dsGeometryPassShader_.setColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		
		if (!dsPointLightShader_.init()) {
			std::cerr << "Failed to initialize point lights" << std::endl;
			return false;
		}

		dsPointLightShader_.enable();
		dsPointLightShader_.SetPositionTextureUnit(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
		dsPointLightShader_.SetColorTextureUnit(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
		dsPointLightShader_.SetNormalTextureUnit(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
		dsPointLightShader_.SetScreenSize(WindowWidth, WindowHeight);

		if (!dsDirLightPassShader_.init()) {
			std::cerr << "Failed to setup dir light" << std::endl;
			return false;
		}

		dsDirLightPassShader_.enable();
		dsDirLightPassShader_.SetPositionTextureUnit(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
		dsDirLightPassShader_.SetColorTextureUnit(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
		dsDirLightPassShader_.SetNormalTextureUnit(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
		dsDirLightPassShader_.SetDirectionalLight( dirLight_ );
		dsDirLightPassShader_.SetScreenSize(WindowWidth, WindowHeight);

		if (!quad_.loadMesh("../Content/quad.obj"))
			return false;

		if (!box_.loadMesh("../Content/box.obj"))
			return false;

		if (!sphere_.loadMesh("../Content/sphere.obj"))
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
		beginLightPass();
		pointLightPass();
		dirLightPass();

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
