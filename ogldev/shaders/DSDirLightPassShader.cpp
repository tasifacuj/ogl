#include "DSDirLightPassShader.hpp"

bool DSDirLightPassShader::init() {
	if (!Technique::init())
		return false;

	if (!loadShader(GL_VERTEX_SHADER, "shaders/t36_light_pass_vs.glsl"))
		return false;

	if (!loadShader(GL_FRAGMENT_SHADER, "shaders/t36_dir_light_pas_fs.glsl"))
		return false;

	if (!finalize())
		return false;

	dirLightLocation_.Color = getUniformLocation("gDirectionalLight.Base.Color");
	dirLightLocation_.AmbientIntensity = getUniformLocation("gDirectionalLight.Base.AmbientIntensity");
	dirLightLocation_.Direction = getUniformLocation("gDirectionalLight.Direction");
	dirLightLocation_.DiffuseIntensity = getUniformLocation("gDirectionalLight.Base.DiffuseIntensity");

	if (dirLightLocation_.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
		dirLightLocation_.Color == INVALID_UNIFORM_LOCATION ||
		dirLightLocation_.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
		dirLightLocation_.Direction == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	return DSLightPassShader::init();
}

void DSDirLightPassShader::SetDirectionalLight(const DirectionalLight& Light)
{
	glUniform3f(dirLightLocation_.Color, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(dirLightLocation_.AmbientIntensity, Light.AmbientIntensity);
	Vector3f Direction = Light.Direction;
	Direction.Normalize();
	glUniform3f(dirLightLocation_.Direction, Direction.x, Direction.y, Direction.z);
	glUniform1f(dirLightLocation_.DiffuseIntensity, Light.DiffuseIntensity);
}
