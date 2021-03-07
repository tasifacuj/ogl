#version 410 core

layout( triangles, equal_spacing, ccw ) in;

uniform mat4		gVP;
uniform sampler2D	gDisplacementMap;
uniform	float		gDispFactor;

in vec3 WorldPos_ES_in[];
in vec2 TexCoord_ES_in[];
in vec3 Normal_ES_in[];

out vec3 WorldPos_FS_in;
out vec2 TexCoord_FS_in;
out vec3 Normal_FS_in;

vec2 interpolate_2d( vec2 v0, vec2 v1, vec2 v2 ){
	return vec2( gl_TessCoord.x ) * v0 + vec2( gl_TessCoord.y ) * v1 + vec2( gl_TessCoord.z ) * v2;
}

vec3 interpolate_3d( vec3 v0, vec3 v1, vec3 v2 ){
	return vec3( gl_TessCoord.x ) * v0 + vec3( gl_TessCoord.y ) * v1 + vec3( gl_TessCoord.z ) * v2;
}

void main(){
	// interpolate attributes of the output vertex, using barycentric coordinates
	TexCoord_FS_in = interpolate_2d( TexCoord_ES_in[ 0 ], TexCoord_ES_in[ 1 ], TexCoord_ES_in[ 2 ] );
	Normal_FS_in = interpolate_3d( Normal_ES_in[ 0 ], Normal_ES_in[ 1 ], Normal_ES_in[ 2 ] );
	Normal_FS_in = normalize( Normal_FS_in );
	WorldPos_FS_in = interpolate_3d( WorldPos_ES_in[ 0 ], WorldPos_ES_in[ 1 ], WorldPos_ES_in[ 2 ] );

	// displace vertex along the normal
	float displacement = texture( gDisplacementMap, TexCoord_FS_in.xy ).x;
	WorldPos_FS_in += Normal_FS_in * displacement * gDispFactor;

	gl_Position = gVP * vec4( WorldPos_FS_in, 1.0 );
}