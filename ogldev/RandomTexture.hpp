#pragma once
#include <GL/glew.h>

class RandomTexture {
	GLuint	textureId_{};
public:
	RandomTexture() = default;
	~RandomTexture();
public:
	bool initialize(unsigned size);
	void bind(GLenum textureUnit);
};