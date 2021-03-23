#include "DSGeometryPassShader.hpp"
#include "../Utils.hpp"

bool DSGeometryPassShader::init() {
	if (!Technique::init())
		return false;

	if (!loadShader(GL_VERTEX_SHADER, "shaders/t36_geometry_pass_vs.glsl"))
		return false;

	if (!loadShader(GL_FRAGMENT_SHADER, "shaders/t36_geometry_pass_fs.glsl"))
		return false;

	if (!finalize())
		return false;

	WVPLocation_ = getUniformLocation("gWVP");
	worldMAtrixLocation_ = getUniformLocation("gWorld");
	colorTextureUnitLocation_ = getUniformLocation("gColorMap");

	if (INVALID_UNIFORM_LOCATION == WVPLocation_ || INVALID_UNIFORM_LOCATION == worldMAtrixLocation_ || INVALID_UNIFORM_LOCATION == colorTextureUnitLocation_)
		return false;

	return true;
}

void DSGeometryPassShader::setWVP(Matrix4f const& wvp) {
	glUniformMatrix4fv(WVPLocation_, 1, GL_TRUE, (const GLfloat*)wvp.m);
}

void DSGeometryPassShader::setWorldMatrix(Matrix4f const& w) {
	glUniformMatrix4fv(worldMAtrixLocation_, 1, GL_TRUE, (const GLfloat*)w.m);
}

void DSGeometryPassShader::setColorTextureUnit(unsigned tu) {
	glUniform1i(colorTextureUnitLocation_, tu);
}
