#include "SimpleColorShader.hpp"
#include "../Utils.hpp"

static constexpr const char* pVS = R"(
#version 330 core

layout ( location = 0 ) in vec3 Position;

uniform mat4 gWVP;

void main(){
	gl_Position = gWVP * vec4( Position, 1.0 );
}

)";

static constexpr const char* pFS = R"(
#version 330 core

layout(location = 0) out vec4 FragColor;

void main(){
	FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}

)";

bool SimpleColorShader::init() {
	if (!Technique::init())
		return false;

	if (!addShader(GL_VERTEX_SHADER, pVS))
		return false;

	if (!addShader(GL_FRAGMENT_SHADER, pFS))
		return false;

	if (!finalize())
		return false;

	wvpLocation_ = getUniformLocation("gWVP");

	if (INVALID_UNIFORM_LOCATION == wvpLocation_)
		return false;

	return true;
}

void SimpleColorShader::setWVP(const Matrix4f& wvp) {
	glUniformMatrix4fv(wvpLocation_, 1, GL_TRUE, (const GLfloat*)wvp.m);
}