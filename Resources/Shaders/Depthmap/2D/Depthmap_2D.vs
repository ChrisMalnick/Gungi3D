/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Depthmap_2D.vs
 * 
 * Summary:	Outputs the depth value of a fragment from a light's perspective
 *		relative to a single direction in clip space
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

layout (location = 0) in vec3 vert_pos;

uniform mat4 u_model;
uniform mat4 u_transform;

void main()
{
	gl_Position = u_transform * u_model * vec4(vert_pos, 1.0f);
}
