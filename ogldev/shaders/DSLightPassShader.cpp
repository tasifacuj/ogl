#include "DSLightPassShader.hpp"

bool DSLightPassShader::init() {
	wvpLocation_ = getUniformLocation("gWVP");
	posTextureUnitLocation_ = getUniformLocation("gPositionMap");
	colorTextureUnitLocation_ = getUniformLocation("gColorMap");
	normalTextureUnitLocation_ = getUniformLocation("gNormalMap");
	eyeWorldPosLocation_ = getUniformLocation("gEyeWorldPos");
	matSpecularIntensityLocation_ = getUniformLocation("gMatSpecularIntensity");
	matSpecularPowerLocation_ = getUniformLocation("gSpecularPower");
	screenSizeLocation_ = getUniformLocation("gScreenSize");

	if (wvpLocation_ == INVALID_UNIFORM_LOCATION ||
		posTextureUnitLocation_ == INVALID_UNIFORM_LOCATION ||
		colorTextureUnitLocation_ == INVALID_UNIFORM_LOCATION ||
		normalTextureUnitLocation_ == INVALID_UNIFORM_LOCATION ||
		eyeWorldPosLocation_ == INVALID_UNIFORM_LOCATION ||
		matSpecularIntensityLocation_ == INVALID_UNIFORM_LOCATION ||
		matSpecularPowerLocation_ == INVALID_UNIFORM_LOCATION ||
		screenSizeLocation_ == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	return true;
}

void DSLightPassShader::SetWVP(const Matrix4f& WVP)
{
	glUniformMatrix4fv(wvpLocation_, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void DSLightPassShader::SetPositionTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(posTextureUnitLocation_, TextureUnit);
}

void DSLightPassShader::SetColorTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(colorTextureUnitLocation_, TextureUnit);
}

void DSLightPassShader::SetNormalTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(normalTextureUnitLocation_, TextureUnit);
}

void DSLightPassShader::SetEyeWorldPos(const Vector3f& EyePos)
{
	glUniform3f(eyeWorldPosLocation_, EyePos.x, EyePos.y, EyePos.z);
}

void DSLightPassShader::SetScreenSize(unsigned int Width, unsigned int Height)
{
	glUniform2f(screenSizeLocation_, (float)Width, (float)Height);
}
