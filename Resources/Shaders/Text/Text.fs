/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Text.fs
 * 
 * Summary:	Outputs the color value of a fragment as the product of a grayscale
 *		texture sample orthographic to the viewport
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

in vec2 frag_UV;

out vec4 result;

struct Text
{
	sampler2D alpha_map;
	vec4 color;
};

uniform Text u_text;

void main()
{
	result = u_text.color * vec4(1.0f, 1.0f, 1.0f, texture(u_text.alpha_map, frag_UV).r);
}
