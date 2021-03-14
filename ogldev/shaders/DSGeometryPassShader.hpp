#pragma once
#include "../math_3d.h"
#include "../Technique.hpp"

class DSGeometryPassShader : public Technique {
	GLuint	WVPLocation_{};
	GLuint	worldMAtrixLocation_{};
	GLuint	colorTextureUnitLocation_{};
public: // == Technique ==
	virtual bool init() override;
public: // == DSGeometryPassShader  ==
	void setWVP(Matrix4f const& vwp);
	void setWorldMatrix(Matrix4f const& w);
	void setColorTextureUnit(unsigned tu);
};