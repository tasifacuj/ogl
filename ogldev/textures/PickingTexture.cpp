#include "PickingTexture.hpp"
#include "../Utils.hpp"

#include <iostream>

PickingTexture::~PickingTexture() {
	if (0 != fbo_)
		glDeleteFramebuffers(1, &fbo_);

	if (0 != pickingTextureId_)
		glDeleteTextures(1, &pickingTextureId_);

	if (0 != depthTextureId_)
		glDeleteTextures(1, &depthTextureId_);
}

bool PickingTexture::init(unsigned wndWidth, unsigned wndHeight) {
	glGenFramebuffers(1, &fbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

	// Create the texture object for the primitive information buffer
	glGenTextures(1, &pickingTextureId_);
	glBindTexture(GL_TEXTURE_2D, pickingTextureId_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, wndWidth, wndHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTextureId_, 0);

	// Create the texture object for the depth buffer
	glGenTextures(1, &depthTextureId_);
	glBindTexture(GL_TEXTURE_2D, depthTextureId_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, wndWidth, wndHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureId_, 0);

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Verify that the FBO is correct
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// Restore the default framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return GLCheckError();
}

void PickingTexture::enableWriting() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);
}

void PickingTexture::disableWriting() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

PickingTexture::PixelInfo PickingTexture::readPixel(unsigned x, unsigned y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	PixelInfo pixel;
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return pixel;
}