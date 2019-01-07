/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Depthmap_2D.fs
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

uniform float u_alpha;

void main()
{
	if (u_alpha < 0.75f)
		discard;

	gl_FragDepth = gl_FragCoord.z;
}
