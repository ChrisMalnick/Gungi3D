/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Depthmap_Cubemap.fs
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

in vec4 frag_pos;

uniform vec3 u_light_pos;
uniform float u_far_plane;

void main()
{
	gl_FragDepth = length(frag_pos.xyz - u_light_pos) / u_far_plane;
}
