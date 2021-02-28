#include "ParticleSystem.hpp"
#include "EngineCommon.hpp"
#include "Utils.hpp"
#include "math_3d.h"

#include <cstring>
#include <iostream>

#define MAX_PARTICLES					1000
#define PARTICLE_LIFETIME				10.0f
#define PARTICLE_TYPE_LAUNCHER			0.0f
#define PARTICLE_TYPE_SHELL				1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL	2.0f

struct Particle {
	float		Type;
	Vector3f	Pos;
	Vector3f	Vel;
	float		LifetimeMillis;
};

ParticleSystem::ParticleSystem()
	: isFirst_(true)
	, currVB_(0)
	, currTFB_(1)
	, time_(0) {
	memset(transformFeedback_, 0, sizeof(transformFeedback_));
	memset(particleBuffer_, 0, sizeof(particleBuffer_));
}

ParticleSystem::~ParticleSystem() {
	if (0 != transformFeedback_[0])
		glDeleteTransformFeedbacks(2, transformFeedback_);

	if (0 != particleBuffer_[0])
		glDeleteBuffers(2, particleBuffer_);
}

bool ParticleSystem::init(Vector3f const& pos) {
	Particle particles[MAX_PARTICLES];
	memset(particles, 0, sizeof(particles));

	particles[0].Type = PARTICLE_TYPE_LAUNCHER;
	particles[0].Pos = pos;
	particles[0].Vel = Vector3f(0.0f, 0.0001f, 0.0f);
	particles[0].LifetimeMillis = 0.0f;

	glGenTransformFeedbacks(2, transformFeedback_);
	glGenBuffers(2, particleBuffer_);

	for (size_t idx = 0; idx < 2; idx++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback_[idx]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer_[idx]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_[idx]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
	}

	if (!psUpdateShader_.init()) {
		std::cout << "psUpdateShader_.init failed" << std::endl;
		return false;
	}

	psUpdateShader_.enable();
	psUpdateShader_.setRandomTextureUnit(RANDOM_TEXTURE_UNIT_INDEX);
	psUpdateShader_.setLauncherLifeTime(100.0f);
	psUpdateShader_.setShellLifeTime(10000.0f);
	psUpdateShader_.setSecondaryShellLifeTime(2500.0f);

	if (!randomTexture_.initialize(1000))
		return false;

	randomTexture_.bind(RANDOM_TEXTURE_UNIT);

	if (!billboardShader_.init())
		return false;

	billboardShader_.enable();
	billboardShader_.setColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
	billboardShader_.setBillboardSize( 0.5f );

	pTexture_ = std::make_unique< Texture >(GL_TEXTURE_2D, "../Content/fireworks_red.jpg");

	if (!pTexture_->load())
		return false;

	return GLCheckError();
}

void ParticleSystem::render(int deltaTimeMs, Matrix4f const& viewProjection, Vector3f const& cameraPos) {
	time_ += deltaTimeMs;
	updateParticles(deltaTimeMs);
	renderParticles(viewProjection, cameraPos);
	currVB_ = currTFB_;
	currTFB_ = (currTFB_ + 1) & 0x1;
}

void ParticleSystem::updateParticles(int deltaTimeMs) {
	psUpdateShader_.enable();
	psUpdateShader_.setTime(time_);
	psUpdateShader_.setDeltaTimeMillis(deltaTimeMs);
	randomTexture_.bind(RANDOM_TEXTURE_UNIT);

	glEnable(GL_RASTERIZER_DISCARD);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_[currVB_]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback_[currTFB_]);
	
	glEnableVertexAttribArray(0);// type
	glEnableVertexAttribArray(1);// pos
	glEnableVertexAttribArray(2);// Velocity
	glEnableVertexAttribArray(3);// age

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);// type
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);// position
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);// velocity
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);// age

	glBeginTransformFeedback(GL_POINTS);

	if (isFirst_) {
		glDrawArrays(GL_POINTS, 0, 1);
		isFirst_ = false;
	}else {
		glDrawTransformFeedback(GL_POINTS, transformFeedback_[currVB_]);
	}

	glEndTransformFeedback();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

void ParticleSystem::renderParticles(Matrix4f const& viewProjection, Vector3f const& cameraPos) {
	billboardShader_.enable();
	billboardShader_.setCameraPosition(cameraPos);
	billboardShader_.setVP(viewProjection);
	pTexture_->bind(COLOR_TEXTURE_UNIT);
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer_[currTFB_]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);//position
	glDrawTransformFeedback(GL_POINTS, transformFeedback_[currTFB_]);
	glDisableVertexAttribArray(0);
}