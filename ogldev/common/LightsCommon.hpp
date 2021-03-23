#pragma once
#include "../math_3d.h"

struct BaseLight {
	Vector3f    Color;
	float       AmbientIntensity;
	float       DiffuseIntensity;

	BaseLight()
		: Color(0.0f, 0.0f, 0.0f)
		, AmbientIntensity(0.0f)
		, DiffuseIntensity(0.0f)
	{}
};



struct DirectionalLight : BaseLight {
	Vector3f    Direction;

	DirectionalLight()
		: Direction(0.0f, 0.0f, 0.0f)
	{}
};

struct PointLight : BaseLight {
	Vector3f    Position;

	struct {
		float Constant;
		float Linear;
		float Exp;
	}Attenuation;

	PointLight()
		: Position(0.0f, 0.0f, 0.0f) {
		Attenuation.Constant = 1.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}
};


struct SpotLight : PointLight {
	Vector3f    Direction;
	float       CutOff;//angle

	SpotLight()
		: Direction(0.0f, 0.0f, 0.0f)
		, CutOff(0.0f)
	{}
};

#define COLOR_WHITE Vector3f(1.0f, 1.0f, 1.0f)
#define COLOR_RED Vector3f(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Vector3f(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN Vector3f(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE Vector3f(0.0f, 0.0f, 1.0f)
