/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Model.fs
 * 
 * Summary:	Outputs the color value of a fragment by performing light and
 *		shadow computations on a texture sample in clip space
 * 
 * Origin:	N/A
 * 
 * Legal:	Unregistered Copyright (C) 2018 Chris Malnick - All Rights Reserved
 *		Unauthorized duplication, reproduction, modification, and/or
 *		distribution is strictly prohibited
 *		All materials, including, but not limited to, code, resources
 *		(models, textures, etc.), documents, etc. are deliberately
 *		unlicensed
 * ============================================================================
 */

#version 330 core

float calcShadow();

float sampleCube();
float sample2D();

float linstep(float p_low, float p_high, float v);

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float brightness;
};

struct Material
{
	sampler2D diffuse;
	float shininess;
};

struct Depthmap
{
	samplerCube sampler_cube;
	sampler2D sampler_2D;

	bool sample_cube;
	bool sample_2D;
};

in vec3 frag_pos;
in vec2 frag_UV;
in vec3 frag_norm;
in vec4 frag_trans;

out vec4 result;

uniform Light u_light;
uniform Material u_material;
uniform Depthmap u_depthmap;
uniform vec3 u_view_pos;
uniform float u_far_plane;
uniform float u_alpha;
uniform bool u_occlude;

void main()
{
	vec3 incidence = u_light.position - frag_pos;
	
	vec3 normal = normalize(frag_norm);
	vec3 view_dir = normalize(u_view_pos - frag_pos);

	// Reflection
	vec3 light_dir = normalize(incidence);
	vec3 halfway_dir = normalize(light_dir + view_dir);

	// Attenuation
	float dist = length(incidence);
	float attenuation = 1.0f / (u_light.constant + u_light.linear * dist + u_light.quadratic * dist * dist);
	
	// Light
	vec3 ambient = (u_light.ambient + u_light.brightness) * attenuation;
	vec3 diffuse = (u_light.diffuse + u_light.brightness) * max(dot(normal, light_dir), 0.0f) * attenuation;
	vec3 specular = (u_light.specular + u_light.brightness) * pow(max(dot(normal, halfway_dir), 0.0f), u_material.shininess) * attenuation;

	// Shadow
	float shadow = (u_occlude ? calcShadow() : 1.0f);

	// Blinn-Phong
	vec3 color = texture(u_material.diffuse, frag_UV).rgb * (ambient + (diffuse + specular) * shadow);

	// Alpha compositing
	result = vec4(color, u_alpha);
}

float calcShadow()
{
	float shadow_cube = (u_depthmap.sample_cube ? sampleCube() : 1.0f);
	float shadow_2D = (u_depthmap.sample_2D ? sample2D() : 1.0f);
	
	return min(shadow_cube, shadow_2D);
}

float sampleCube()
{
	vec3 incidence = frag_pos - u_light.position;

	float dist = length(incidence);
	float depth = texture(u_depthmap.sampler_cube, incidence).r * u_far_plane;

	float dx = dFdx(depth);
	float dy = dFdy(depth);

	float moment = depth * depth + (dx * dx + dy * dy) * 0.25f;

	float variance = max(moment - depth * depth, 0.00002f);
	float d = dist - depth;

	float p = smoothstep(dist - 0.02f, dist, depth);
	float p_max = linstep(0.2f, 1.0f, variance / (variance + d * d));

	return clamp(max(p, p_max), 0.0f, 1.0f);
}

float sample2D()
{
	vec3 projection = (frag_trans.xyz / frag_trans.w) * 0.5f + 0.5f;

	float dist = projection.z;
	float depth = texture(u_depthmap.sampler_2D, projection.xy).r;

	return (1.0f - float(dist - 0.00002f > depth));
}

float linstep(float p_low, float p_high, float v)
{
	return clamp((v - p_low) / (p_high - p_low), 0.0f, 1.0f);
}
