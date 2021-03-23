#include "DSPointLightShader.hpp"

bool DSPointLightShader::init() {
	if (!Technique::init())
		return false;

	if (!loadShader(GL_VERTEX_SHADER, "shaders/t36_light_pass_vs.glsl"))
		return false;

	if (!loadShader(GL_FRAGMENT_SHADER, "shaders/t36_point_light_pass_fs.glsl"))
		return false;

	if (!finalize())
		return false;

	pointLightLocation_.Color = getUniformLocation("gPointLight.Base.Color");
	pointLightLocation_.AmbientIntensity = getUniformLocation("gPointLight.Base.AmbientIntensity");
	pointLightLocation_.Position = getUniformLocation("gPointLight.Position");
	pointLightLocation_.DiffuseIntensity = getUniformLocation("gPointLight.Base.DiffuseIntensity");
	pointLightLocation_.Atten.Constant = getUniformLocation("gPointLight.Atten.Constant");
	pointLightLocation_.Atten.Linear = getUniformLocation("gPointLight.Atten.Linear");
	pointLightLocation_.Atten.Exp = getUniformLocation("gPointLight.Atten.Exp");

	if (pointLightLocation_.Color == INVALID_UNIFORM_LOCATION ||
		pointLightLocation_.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
		pointLightLocation_.Position == INVALID_UNIFORM_LOCATION ||
		pointLightLocation_.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
		pointLightLocation_.Atten.Constant == INVALID_UNIFORM_LOCATION ||
		pointLightLocation_.Atten.Linear == INVALID_UNIFORM_LOCATION ||
		pointLightLocation_.Atten.Exp == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	return DSLightPassShader::init();
}

void DSPointLightShader::setPointLight(PointLight const& Light) {
	glUniform3f(pointLightLocation_.Color, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(pointLightLocation_.AmbientIntensity, Light.AmbientIntensity);
	glUniform1f(pointLightLocation_.DiffuseIntensity, Light.DiffuseIntensity);
	glUniform3f(pointLightLocation_.Position, Light.Position.x, Light.Position.y, Light.Position.z);
	glUniform1f(pointLightLocation_.Atten.Constant, Light.Attenuation.Constant);
	glUniform1f(pointLightLocation_.Atten.Linear, Light.Attenuation.Linear);
	glUniform1f(pointLightLocation_.Atten.Exp, Light.Attenuation.Exp);
}