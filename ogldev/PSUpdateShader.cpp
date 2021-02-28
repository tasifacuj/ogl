#include "PSUpdateShader.hpp"

#include <iostream>

static constexpr const char* pVs = R"(
#version 330 core

layout ( location = 0) in float Type;
layout ( location = 1 ) in vec3 Position;
layout ( location = 2 ) in vec3 Velocity;
layout ( location = 3) in float Age;

out float Type0;
out vec3 Position0;
out vec3 Velocity0;
out float Age0;

void main(){
	Type0 = Type;
	Position0 = Position;
	Velocity0 = Velocity;
	Age0 = Age;
}

)";

static constexpr const char* pGS = R"(
#version 330 core

layout( points ) in;
layout( points ) out;
layout( max_vertices = 30 ) out;

in float	Type0[];
in vec3		Position0[];
in vec3		Velocity0[];
in float	Age0[];

out float	Type1;
out vec3	Position1;
out vec3	Velocity1;
out float	Age1;

uniform float		gDeltaTimeMillis;
uniform float		gTime;
uniform sampler1D	gRandomTexture;
uniform float		gLauncherLifeTime;
uniform float		gShellLifeTime;
uniform float		gSecondaryShellLifeTime;

#define PARTICLE_TYPE_LAUNCHER			0.0
#define PARTICLE_TYPE_SHELL				1.0
#define PARTICLE_TYPE_SECONDARY_SHELL	2.0

vec3 get_random_dir( float texCoord ){
	vec3 dir = texture( gRandomTexture, texCoord ).xyz;
	dir -= vec3( 0.5, 0.5, 0.5 );
	return dir;
}

void main(){
	float age = Age0[ 0 ] + gDeltaTimeMillis;

	if( Type0[ 0 ] == PARTICLE_TYPE_LAUNCHER ){
		if( age >= gLauncherLifeTime ){
			Type1 = PARTICLE_TYPE_SHELL;
			Position1 = Position0[0];
            vec3 dir = get_random_dir( gTime / 1000.0 );
			dir.y = max( dir.y, 0.5 );
			Velocity1 = normalize( dir ) / 20.0;
			Age1 = 0.0;
			EmitVertex();
			EndPrimitive();
			age = 0.0;
		}

		Type1 = PARTICLE_TYPE_LAUNCHER;
		Position1 = Position0[ 0 ];
		Velocity1 = Velocity0[ 0 ];
		Age1 = age;
		EmitVertex();
		EndPrimitive();
	}else{ // end PARTICLE_TYPE_LAUNCHER
		float deltaTimeSec = gDeltaTimeMillis / 1000.0f;
		float t1 = Age0[ 0 ] / 1000.0f;
		float t2 = age / 1000.0f;
		vec3 deltaP = deltaTimeSec * Velocity0[ 0 ];// distance
		vec3 deltaV = vec3( deltaTimeSec ) * ( 0.0, -9.81, 0.0 );

		if( Type0[ 0 ] == PARTICLE_TYPE_SHELL ){
			if( age < gShellLifeTime ){
				Type1 = PARTICLE_TYPE_SHELL;
				Position1 = Position0[ 0 ] + deltaP;
				Velocity1 = Velocity0[ 0 ] + deltaV;
				Age1 = age;
				EmitVertex();
				EndPrimitive();
			}else{
				for( int i = 0; i < 10; i++ ){
					Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
					Position1 = Position0[ 0 ];
					vec3 dir = get_random_dir( ( gTime + i ) / 1000.0 );
					Velocity1 = normalize( dir ) / 20.0;
					Age1 = 0.0f;
					EmitVertex();
					EndPrimitive();
				}
			}
		}else{ // PARTICLE_TYPE_SECONDARY_SHELL
			if( age < gSecondaryShellLifeTime ){
				Type1 = PARTICLE_TYPE_SECONDARY_SHELL;
				Position1 = Position0[ 0 ] + deltaP;
				Velocity1 = Velocity0[ 0 ] + deltaV;
				Age1 = age;
				EmitVertex();
				EndPrimitive();
			}
		}
	}
}

)";

static constexpr const char* pFS = R"(
#version 330

void main()
{
}

)";

bool PSUpdateShader::init() {
	if (!Technique::init())
		return false;

	if (!addShader(GL_VERTEX_SHADER, pVs))
		return false;

	if (!addShader(GL_GEOMETRY_SHADER, pGS))
		return false;

	if (!addShader(GL_FRAGMENT_SHADER, pFS))
		return false;

	const GLchar* varyings[4] = {
		"Type1",
		"Position1",
		"Velocity1",
		"Age1"
	};

	glTransformFeedbackVaryings(shaderProgram_, 4, varyings, GL_INTERLEAVED_ATTRIBS);

	if (!finalize()) {
		std::cerr << "PSUpdateShader::finalize failed" << std::endl;
		return false;
	}

// get locations
	deltaTimeMillisLocation_ = getUniformLocation("gDeltaTimeMillis");
	randomTextureLocation_ = getUniformLocation("gRandomTexture");
	timeLocation_ = getUniformLocation("gTime");
	launcherLeftTimeLocation_ = getUniformLocation("gLauncherLifeTime");
	shellLifeTimeLocation_ = getUniformLocation("gShellLifeTime");
	secondaryShellLifeTimeLocation_ = getUniformLocation("gSecondaryShellLifeTime");

	if (INVALID_UNIFORM_LOCATION == deltaTimeMillisLocation_
		|| INVALID_UNIFORM_LOCATION == randomTextureLocation_
		|| INVALID_UNIFORM_LOCATION == timeLocation_
		|| INVALID_UNIFORM_LOCATION == launcherLeftTimeLocation_
		|| INVALID_UNIFORM_LOCATION == shellLifeTimeLocation_
		|| INVALID_UNIFORM_LOCATION == secondaryShellLifeTimeLocation_
		) {
		return false;
	}

	return true;
}

void PSUpdateShader::setDeltaTimeMillis(float dt) {
	glUniform1f(deltaTimeMillisLocation_, dt);
}

void PSUpdateShader::setTime(int t) {
	glUniform1f(timeLocation_, t);
}

void PSUpdateShader::setRandomTextureUnit(unsigned textureUnitId) {
	glUniform1i(randomTextureLocation_, textureUnitId);
}

void PSUpdateShader::setLauncherLifeTime(float lifeTime) {
	glUniform1f(launcherLeftTimeLocation_, lifeTime);
}

void PSUpdateShader::setShellLifeTime(float lifeTime) {
	glUniform1f(shellLifeTimeLocation_, lifeTime);
}

void PSUpdateShader::setSecondaryShellLifeTime(float lifeTime) {
	glUniform1f(secondaryShellLifeTimeLocation_, lifeTime);
}