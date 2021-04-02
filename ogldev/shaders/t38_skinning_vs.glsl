#version 330 core

layout(location = 0) in vec3 Position; 
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;
layout(location = 3) in ivec4 BoneIDs;
layout(location = 4) in vec4 Weights;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

const int MAX_BONES = 100;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gBones[MAX_BONES];

void main() {
	mat4 boneTransform = gBones[BoneIDs[0]] * Weights[0];
	boneTransform += gBones[BoneIDs[1]] * Weights[1];
	boneTransform += gBones[BoneIDs[2]] * Weights[2];
	boneTransform += gBones[BoneIDs[3]] * Weights[3];

	vec4 posL	= boneTransform * vec4(Position, 1.0);
	gl_Position = gWVP * posL;
	TexCoord0	= TexCoord;
	vec4 normalL= boneTransform * vec4(Normal, 0.0);
	Normal0		= (gWorld * normalL).xyz;
	WorldPos0	= (gWorld * posL).xyz;
}