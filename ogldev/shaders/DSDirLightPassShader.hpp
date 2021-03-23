#pragma once

#include "DSLightPassShader.hpp"
#include "../common/LightsCommon.hpp"

class DSDirLightPassShader final : public DSLightPassShader {
	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Direction;
	}dirLightLocation_;
public:
	DSDirLightPassShader() = default;
public:
	virtual bool init() override;

public:
	void SetDirectionalLight(const DirectionalLight& Light);
};
