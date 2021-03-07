#version 410 core

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec3 WorldPos_FS_in;
in vec2 TexCoord_FS_in;
in vec3 Normal_FS_in;

out vec4 FragColor;

struct BaseLight{
	vec3	Color;
	float	AmbientIntensity;
	float	DiffuseIntensity;
};

struct DirectionalLight{
	BaseLight	Base;
	vec3		Direction;
};

struct Attenuation{
	float	Constant;
	float	Linear;
	float	Exp;
};

struct PointLight{
	BaseLight	Base;
	vec3		Position;
	Attenuation	Atten;
};

struct SpotLight{
	PointLight	Base;
	vec3		Direction;
	float		Cutoff;
};

uniform int					gNumPointLights;
uniform int					gNumSpotLights;
uniform DirectionalLight	gDirectionalLight;
uniform PointLight			gPointLights[ MAX_POINT_LIGHTS ];
uniform SpotLight			gSpotLights[ MAX_SPOT_LIGHTS ];
uniform sampler2D			gColorMap;
uniform vec3				gEyeWorldPos;
uniform float				gMatSpecularIntensity;
uniform float				gSpecularPower;

vec4 calc_light_internal( BaseLight light, vec3 light_dir, vec3 normal ){
	vec4 ambient_color = vec4( light.Color * light.AmbientIntensity, 1.0 );
	float diffuse_factor = dot( normal, -light_dir );
	vec4 diffuse_color = vec4( 0.0, 0.0, 0.0, 0.0 );
	vec4 specular_color = vec4( 0.0, 0.0, 0.0, 0.0 );

	if( diffuse_factor > 0.0 ){
		diffuse_color = vec4( light.Color * light.DiffuseIntensity * diffuse_factor, 1.0 );
		vec3 vertex_2_eye = normalize( gEyeWorldPos - WorldPos_FS_in );
		vec3 light_reflected = normalize( reflect( light_dir, normal ) );
		float specular_factor = dot( vertex_2_eye, light_reflected );

		if( specular_factor > 0 ){
			specular_factor = pow( specular_factor, gSpecularPower );
			specular_color = vec4( light.Color * gMatSpecularIntensity * specular_factor, 1.0 );
		}
	}

	return ( ambient_color + diffuse_color + specular_color );
}

vec4 calc_directional_light( vec3 normal ){
	return calc_light_internal( gDirectionalLight.Base, gDirectionalLight.Direction, normal );
}

vec4 calc_point_light( PointLight l, vec3 normal ){
	vec3 light_dir = WorldPos_FS_in - l.Position;
	float dist = length( light_dir );
	light_dir = normalize( light_dir );

	vec4 color = calc_light_internal( l.Base, light_dir, normal );
	float atten = l.Atten.Constant + l.Atten.Linear * dist + l.Atten.Exp * dist * dist;
	return color / atten;
}

vec4 calc_spot_light( SpotLight l, vec3 normal ){
	vec3 light_2_pixel = normalize( WorldPos_FS_in - l.Base.Position );
	float spt_factor = dot( light_2_pixel, l.Direction );

	if( spt_factor > l.Cutoff ){
		vec4 color = calc_point_light( l.Base, normal );
		return color * (1.0 - (1.0 - spt_factor) * 1.0/(1.0 - l.Cutoff));          
	}else{
		return vec4( 0,0,0,0 );
	}
}

void main(){
	vec3 normal = normalize( Normal_FS_in );
	vec4 total_light = calc_directional_light( normal );

	for( int i = 0; i < gNumPointLights; i++ ){
		total_light += calc_point_light( gPointLights[ i ], normal );
	}

	for( int i = 0; i < gNumSpotLights; i++ ){
		total_light += calc_spot_light( gSpotLights[ i ], normal );
	}

	FragColor = texture( gColorMap, TexCoord_FS_in.xy ) * total_light;
}