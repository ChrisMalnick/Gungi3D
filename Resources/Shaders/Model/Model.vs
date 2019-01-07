/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Model.vs
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

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_UV;
layout (location = 2) in vec3 vert_norm;

out vec3 frag_pos;
out vec2 frag_UV;
out vec3 frag_norm;
out vec4 frag_trans;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_transform;

void main()
{
	frag_pos = vec3(u_model * vec4(vert_pos, 1.0f));
	frag_UV = vert_UV;
	frag_norm = transpose(inverse(mat3(u_model))) * vert_norm;
	frag_trans = u_transform * vec4(frag_pos, 1.0f);

	gl_Position = u_projection * u_view * u_model * vec4(vert_pos, 1.0f);
}
