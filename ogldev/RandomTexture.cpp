#include "RandomTexture.hpp"
#include "math_3d.h"
#include "Utils.hpp"

#include <vector>
#include <cstdlib>
#include <cmath>

static float random_float() {
	float max_val = RAND_MAX;
	return float(rand() / max_val);
}

RandomTexture::~RandomTexture() {
	if (0 != textureId_)
		glDeleteTextures(1, &textureId_);
}

bool RandomTexture::initialize(unsigned size) {
	std::vector< Vector3f > randomData;


	for (size_t idx = 0; idx < size; idx++) {
		randomData.emplace_back(random_float(), random_float(), random_float());
	}

	glGenTextures(1, &textureId_);
	glBindTexture(GL_TEXTURE_1D, textureId_);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0.0f, GL_RGB, GL_FLOAT, &randomData[ 0 ]);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	return GLCheckError();
}

void RandomTexture::bind(GLenum textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_1D, textureId_);
}