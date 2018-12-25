/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Overlay.vs
 *
 * Summary:	Outputs the color value of a fragment as a function of brightness
 *			in clip space
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

layout (location = 0) in vec3 vert_pos;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(vert_pos, 1.0f);
}