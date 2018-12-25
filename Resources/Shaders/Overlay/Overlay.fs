/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Overlay.fs
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

struct Overlay
{
	vec4 color;
	float brightness;
};

uniform Overlay u_overlay;

void main()
{
	gl_FragColor = u_overlay.color * pow(u_overlay.brightness + 0.3f, 0.25f);
}