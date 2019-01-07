/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Light.h
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

#ifndef LIGHT_H
#define LIGHT_H

#include "Renderer/Shader.h"

static const glm::vec3 LIGHT_POSITION = glm::vec3(0.0f, 5.0f, 0.0f);

static const glm::vec3 LIGHT_AMBIENT = glm::vec3(0.3f);
static const glm::vec3 LIGHT_DIFFUSE = glm::vec3(0.2f);
static const glm::vec3 LIGHT_SPECULAR = glm::vec3(0.1f);

static const float LIGHT_CONSTANT = 1.0f;
static const float LIGHT_LINEAR = 0.1f;
static const float LIGHT_QUADRATIC = 0.01f;

static const float LIGHT_RATE = 0.5f;

static const float LIGHT_MIN = 0.1f;
static const float LIGHT_MAX = 0.7f;

class Light {
public:
	// Member functions
	// ----------------
	inline void init() { this->m_brightness = LIGHT_MAX; }

	inline float getBrightness() { return this->m_brightness; }

	void dim(double p_delta_time);

	void setUniforms(const Shader &p_shader_ref);

private:
	// Member variables
	// ----------------
	float m_brightness = LIGHT_MAX;
};

#endif // LIGHT_H
