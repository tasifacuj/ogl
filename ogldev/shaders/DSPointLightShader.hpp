#pragma once
#include "DSLightPassShader.hpp"
#include "../common/LightsCommon.hpp"

class DSPointLightShader : public DSLightPassShader {
	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Position;

		struct{
			GLuint Constant;
			GLuint Linear;
			GLuint Exp;
		} Atten;
	} pointLightLocation_;

public:// == Technique ==
	virtual bool init() override;
public:// == DSPointLightShader ==
	void setPointLight(PointLight const& light);
};