#pragma once

#include "Technique.hpp"

class PSUpdateShader : public Technique {
	GLuint	deltaTimeMillisLocation_{};
	GLuint	randomTextureLocation_{};
	GLuint	timeLocation_{};
	GLuint	launcherLeftTimeLocation_{};
	GLuint	shellLifeTimeLocation_{};
	GLuint	secondaryShellLifeTimeLocation_{};
public:
	PSUpdateShader() = default;
public:// == Technique ==
	virtual bool init() override;
public:// == PSUpdateShader ==
	void setParticleLifeTime(float lifeTime);
	void setDeltaTimeMillis(float deltaTimeMS);
	void setTime(int time);
	void setRandomTextureUnit(unsigned textureUnitId);
	void setLauncherLifeTime(float lifeTime);
	void setShellLifeTime(float shellLifeTime);
	void setSecondaryShellLifeTime(float lifeTime);
};