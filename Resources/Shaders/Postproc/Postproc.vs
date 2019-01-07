/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Postproc.vs
 * 
 * Summary:	Outputs an approximate low dynamic color value by tone mapping and
 *		gamma correcting a high dynamic color sample
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
layout (location = 1) in vec2 vert_UV;

out vec2 frag_UV;

void main()
{
	frag_UV = vert_UV;
	gl_Position = vec4(vert_pos, 1.0f);
}
