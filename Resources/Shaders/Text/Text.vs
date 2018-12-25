/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Text.vs
 *
 * Summary:	Outputs the color value of a fragment as the product of a grayscale
 *			texture sample orthographic to the viewport
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

layout (location = 0) in vec2 vert_pos;
layout (location = 1) in vec2 vert_UV;

out vec2 frag_UV;

uniform mat4 u_projection;

void main()
{
	frag_UV = vert_UV;
	gl_Position = u_projection * vec4(vert_pos, 0.0f, 1.0f);
}