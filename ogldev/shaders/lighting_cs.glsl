#version 410 core

// define CPs in the output patch
layout( vertices = 3 ) out;

uniform vec3 gEyeWorldPos;

// attributes of the input CPs
in vec3 WorldPos_CS_in[];
in vec2 TexCoord_CS_in[];
in vec3 Normal_CS_in[];

// attributes of the output CPs
out vec3 WorldPos_ES_in[];
out vec2 TexCoord_ES_in[];
out vec3 Normal_ES_in[];

float get_tess_level( float d0, float d1 ){
	float avg = ( d0 + d1 ) / 2.0;

	if( avg <= 2.0 )
		return 10.0;
	else if( avg <= 5.0 )
		return 7.0;
	else
		return 3.0;
}

void main(){
	// set control points in the output patch
	TexCoord_ES_in[ gl_InvocationID ] = TexCoord_CS_in[ gl_InvocationID ];
	Normal_ES_in[ gl_InvocationID ] = Normal_CS_in[ gl_InvocationID ];
	WorldPos_ES_in[ gl_InvocationID ] = WorldPos_CS_in[ gl_InvocationID ];

	// calculate distance from camera to control points
	float eye_2_vertex_d0 = distance( gEyeWorldPos, WorldPos_ES_in[ 0 ] );
	float eye_2_vertex_d1 = distance( gEyeWorldPos, WorldPos_ES_in[ 1 ] );
	float eye_2_vertex_d2 = distance( gEyeWorldPos, WorldPos_ES_in[ 2 ] );

	// calculate tesselation levels
	gl_TessLevelOuter[ 0 ] = get_tess_level( eye_2_vertex_d1, eye_2_vertex_d2 ); //  gl_TessLevelOuter[] roughly determines the number of segments on each edge
	gl_TessLevelOuter[ 1 ] = get_tess_level( eye_2_vertex_d2, eye_2_vertex_d0 );
	gl_TessLevelOuter[ 2 ] = get_tess_level( eye_2_vertex_d0, eye_2_vertex_d1 );//  gl_TessLevelInner[0] roughly determines how many rings the triangle will contain
	gl_TessLevelInner[ 0 ] = gl_TessLevelOuter[ 2 ];
}