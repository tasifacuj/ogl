#pragma once
#include "../Technique.hpp"
#include "../math_3d.h"
#include "../interface/RenderCallbackInterface.hpp"

class PickingShader : public Technique, public RenderCallbackInterface {
	GLuint	wvpLocation_{};
	GLuint	drawIndexLocation_{};
	GLuint	objectIndexLocation_{};

public: // == Technique ==
	virtual bool init() override;

public: // == RenderCallbackInterface ==
	virtual void drawStartedCB(unsigned drawIndex) override;

public: // == PickingShader ==
	void setWVP(Matrix4f const& wvp);
	void setObjectIndex(unsigned objIdx);
};

