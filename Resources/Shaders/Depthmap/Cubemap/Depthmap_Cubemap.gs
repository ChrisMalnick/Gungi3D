/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Depthmap_Cubemap.gs
 * 
 * Summary:	Outputs the depth values of a fragment from a light's perspective
 *		relative to the faces of a cubemap in clip space
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

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 frag_pos;

uniform mat4 u_transforms[6];

void main()
{
	for (int i = 0; i < 6; ++i)
	{
		gl_Layer = i;

		for (int j = 0; j < 3; ++j)
		{
			frag_pos = gl_in[j].gl_Position;
			gl_Position = u_transforms[i] * frag_pos;

			EmitVertex();
		}

		EndPrimitive();
	}
}
