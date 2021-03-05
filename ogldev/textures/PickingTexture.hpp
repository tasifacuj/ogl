#pragma once
#include <GL/glew.h>

class PickingTexture final {
public: // == TYPES ==
	struct PixelInfo {
		float ObjectId{ 0.0f };
		float DrawId{ 0.0f };
		float PrimId{ 0.0f };
	};

private: // == MEMBERS ==
	GLuint	fbo_{ 0 };
	GLuint	pickingTextureId_{ 0 };
	GLuint	depthTextureId_{ 0 };
public: // == CTOR ==
	PickingTexture() = default;
	~PickingTexture();
public:
	bool init(unsigned windWidth, unsigned wndHeight);
	void enableWriting();
	void disableWriting();
	PixelInfo readPixel(unsigned x, unsigned y);
};