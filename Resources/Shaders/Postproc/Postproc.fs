/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Postproc.fs
 *
 * Summary:	Outputs an approximate low dynamic color value by tone mapping and
 *			gamma correcting a high dynamic color sample
 *
 * Origin:	N/A
 *
 * Legal:	Unregistered Copyright (C) 2018 Chris Malnick - All Rights Reserved
 *			Unauthorized duplication, reproduction, modification, and/or
 *			distribution is strictly prohibited
 *			All materials, including, but not limited to, code, resources
 *			(models, textures, etc.), documents, etc. are deliberately
 *			unlicensed
 * ============================================================================
 */

#version 330 core

in vec2 frag_UV;

out vec4 result;

uniform sampler2D u_HDR;
uniform float u_exposure;
uniform float u_gamma;
uniform float u_alpha;

void main()
{
	vec3 color = texture(u_HDR, frag_UV).rgb;

	color = vec3(1.0f) - exp(-color * u_exposure); // Tone mapping (HDR -> LDR)
	color = pow(color, vec3(1.0f / u_gamma)); // Gamma correction

	result = vec4(color, u_alpha);
}