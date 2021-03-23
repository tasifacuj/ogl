#pragma once
#include "../Technique.hpp"
#include "../math_3d.h"

class DSLightPassShader : public Technique {
	GLuint	wvpLocation_{};
	GLuint	posTextureUnitLocation_{};
	GLuint	normalTextureUnitLocation_{};
	GLuint	colorTextureUnitLocation_{};
	GLuint	eyeWorldPosLocation_{};
	GLuint	matSpecularIntensityLocation_{};
	GLuint	matSpecularPowerLocation_{};
	GLuint	screenSizeLocation_{};
public:
	DSLightPassShader() = default;
public:// == Technique ==
	virtual bool init() override;
public:// == DSLightPassShader ==
	void SetWVP(const Matrix4f& WVP);
	void SetPositionTextureUnit(unsigned int TextureUnit);
	void SetColorTextureUnit(unsigned int TextureUnit);
	void SetNormalTextureUnit(unsigned int TextureUnit);
	void SetEyeWorldPos(const Vector3f& EyeWorldPos);
	void SetMatSpecularIntensity(float Intensity);
	void SetMatSpecularPower(float Power);
	void SetScreenSize(unsigned int Width, unsigned int Height);
};