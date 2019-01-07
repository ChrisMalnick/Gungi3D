/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Light.cpp
 * 
 * Summary:	Manages the various properties that determine the position,
 *		brightness, and quality of the light relative to the scene
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

#include "World/Light.h"

// Member functions
// ----------------
void Light::dim(double p_delta_time)
{
	this->m_brightness += static_cast<float>(p_delta_time) * LIGHT_RATE;

	if (this->m_brightness < LIGHT_MIN)
		this->m_brightness = LIGHT_MIN;

	else if (this->m_brightness > LIGHT_MAX)
		this->m_brightness = LIGHT_MAX;
}

void Light::setUniforms(const Shader &p_shader_ref)
{
	p_shader_ref.setVec3("u_light.position", LIGHT_POSITION);

	p_shader_ref.setVec3("u_light.ambient", LIGHT_AMBIENT);
	p_shader_ref.setVec3("u_light.diffuse", LIGHT_DIFFUSE);
	p_shader_ref.setVec3("u_light.specular", LIGHT_SPECULAR);

	p_shader_ref.setFloat("u_light.constant", LIGHT_CONSTANT);
	p_shader_ref.setFloat("u_light.linear", LIGHT_LINEAR);
	p_shader_ref.setFloat("u_light.quadratic", LIGHT_QUADRATIC);

	p_shader_ref.setFloat("u_light.brightness", this->m_brightness);
}
