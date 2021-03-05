#pragma once
#include "../Technique.hpp"
#include "../math_3d.h"

class SimpleColorShader : public Technique{
	GLuint wvpLocation_{};
public: // == Technique ==
	virtual bool init() override;
public:// == SimpleColorShader ==
	void setWVP(Matrix4f const& wvp);
};