#pragma once

#include <GL/glew.h>
#include <memory>

#include "PSUpdateShader.hpp"
#include "RandomTexture.hpp"
#include "BillboardShader.hpp"
#include "Texture.hpp"



class ParticleSystem {
	bool						isFirst_{ true };
	unsigned					currVB_{0};
	unsigned					currTFB_{1};
	GLuint						particleBuffer_[2];
	GLuint						transformFeedback_[2];
	PSUpdateShader				psUpdateShader_;
	BillboardShader				billboardShader_;
	RandomTexture				randomTexture_;
	std::unique_ptr< Texture >	pTexture_;
	int							time_{ 0 };
public: // == CTORs ==
	ParticleSystem();
	~ParticleSystem();
public: // == ParticleSystem ==
	bool init(Vector3f const& pos);
	void render(int deltaTimeMs, Matrix4f const& viewProjection, Vector3f const& cameraPos);
private:
	void updateParticles(int deltaTimeMs);
	void renderParticles(Matrix4f const& viewProjection, Vector3f const& cameraPos);
};