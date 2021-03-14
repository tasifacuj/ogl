#pragma once

#include <GL/glew.h>

class GBuffer final {
public: // == CONSTANTS ==
	static constexpr int GBUFFER_POSITION_TEXTURE_UNIT	= 0;
	static constexpr int GBUFFER_DIFFUSE_TEXTURE_UNIT	= 1;
	static constexpr int GBUFFER_NORMAL_TEXTURE_UNIT	= 2;
	static constexpr int GBUFFER_TEXCOORD_TEXTURE_UNIT	= 3;
public: // == TYPES ==
	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};
public: // == CTORs ==
	GBuffer();
	~GBuffer();
public: // == GBUffer ==
	bool init(unsigned width, unsigned height);
	void bindForWriting();
	void bindForReading();
	void setReadBuffer(GBUFFER_TEXTURE_TYPE textureType);
private:
	GLuint	fbo_{0};
	GLuint	textures_[GBUFFER_NUM_TEXTURES] = {};
	GLuint	depthTexture_{ 0 };
};