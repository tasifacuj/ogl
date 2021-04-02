#version 330 core

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

struct VSOutput
{
	vec2 TexCoord;
	vec3 Normal;
	vec3 WorldPos;
};


struct BaseLight
{
	vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
};

struct DirectionalLight
{
	BaseLight Base;
	vec3 Direction;
};

struct Attenuation
{
	float Constant;
	float Linear;
	float Exp;
};

struct PointLight
{
	BaseLight Base;
	vec3 Position;
	Attenuation Atten;
};

struct SpotLight
{
	PointLight Base;
	vec3 Direction;
	float Cutoff;
};

uniform int					gNumPointLights;
uniform int					gNumSpotLights;
uniform DirectionalLight	gDirectionalLight;
uniform PointLight			gPointLights[MAX_POINT_LIGHTS];
uniform SpotLight			gSpotLights[MAX_SPOT_LIGHTS];
uniform sampler2D			gColorMap;
uniform vec3				gEyeWorldPos;
uniform float				gMatSpecularIntensity;
uniform float				gSpecularPower;

vec4 CalcLightInternal(BaseLight light, vec3 lightDirection, VSOutput inParam) {
	vec4 ambientColor = vec4(light.Color * light.AmbientIntensity, 1.0);
	float diffuseFactor = dot(inParam.Normal, -lightDirection);

	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec4 specularColor = vec4(0, 0, 0, 0);

	if (diffuseFactor > 0.0) {
		diffuseColor = vec4(light.Color * light.DiffuseIntensity * diffuseFactor, 1.0);
		vec3 vertex2eye = normalize(gEyeWorldPos - inParam.WorldPos);
		vec3 lightReflected = normalize(reflect(lightDirection, inParam.Normal));
		float specularFactor = dot(vertex2eye, lightReflected);

		if (specularFactor > 0.0) {
			specularFactor = pow(specularFactor, gSpecularPower);
			specularColor = vec4(light.Color * gMatSpecularIntensity *  specularFactor, 1.0);
		}
	}

	return (ambientColor + diffuseColor + specularColor);
}

vec4 CalcDirectionalLight(VSOutput inParams) {
	return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, inParams);
}

vec4 CalcPointLight(PointLight l, VSOutput inParams) {
	vec3 lightDir = inParams.WorldPos - l.Position;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);
	vec4 color = CalcLightInternal(l.Base, lightDir, inParams);
	float atten = l.Atten.Constant + l.Atten.Linear * dist + l.Atten.Exp * dist * dist;
	return color / atten;
}

vec4 CalcSpotLight(SpotLight l, VSOutput inParams) {
	vec3 light2pixel = normalize(inParams.WorldPos - l.Base.Position);
	float spotFactor = dot(light2pixel, l.Direction);

	if (spotFactor > l.Cutoff) {
		vec4 color = CalcPointLight(l.Base, inParams);
		return color * (1.0 - (1.0 - spotFactor) * 1.0 / (1.0 - l.Cutoff));
	}else {
		return vec4(0, 0, 0, 0);
	}
}

out vec4 FragColor;

void main() {
	VSOutput inParams;
	inParams.TexCoord = TexCoord0;
	inParams.Normal = normalize(Normal0);
	inParams.WorldPos = WorldPos0;

	vec4 totalLight = CalcDirectionalLight(inParams);

	for (int idx = 0; idx < gNumPointLights; idx++) {
		totalLight += CalcPointLight(gPointLights[idx], inParams);
	}

	for (int idx = 0; idx < gNumSpotLights; idx++) {
		totalLight += CalcSpotLight(gSpotLights[idx], inParams);
	}

	FragColor = texture(gColorMap, inParams.TexCoord.xy) * totalLight;
}