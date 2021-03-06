#include "GBuffer.hpp"
#include "../Utils.hpp"
#include "../Texture.hpp"

#include <cstring>

GBuffer::GBuffer()
	: fbo_(0)
	, depthTexture_(0) {
	memset(textures_, 0, sizeof(textures_));
}

GBuffer::~GBuffer() {
	if (0 != fbo_)
		glDeleteFramebuffers(1, &fbo_);

	if (0 != textures_[0])
		glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(textures_), textures_);

	if (0 != depthTexture_)
		glDeleteTextures(1, &depthTexture_);
}

bool GBuffer::init(unsigned width, unsigned height) {
	glGenFramebuffers(1, &fbo_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);

	// Create the gbuffer textures
	glGenTextures(ARRAY_SIZE_IN_ELEMENTS(textures_), textures_);
	glGenTextures(1, &depthTexture_);

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(textures_); i++) {
		glBindTexture(GL_TEXTURE_2D, textures_[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures_[i], 0);
	}

	// depth
	glBindTexture(GL_TEXTURE_2D, depthTexture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture_, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
							 GL_COLOR_ATTACHMENT1,
							 GL_COLOR_ATTACHMENT2,
							 GL_COLOR_ATTACHMENT3
	};

	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::bindForWriting() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
}

void GBuffer::bindForReading() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	for (size_t idx = 0; idx < ARRAY_SIZE_IN_ELEMENTS( textures_ ); idx++) {
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, textures_[GBUFFER_TEXTURE_TYPE_POSITION + idx]);
	}
}

void GBuffer::setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE tt) {
	glReadBuffer(GL_COLOR_ATTACHMENT0 + tt);
}
