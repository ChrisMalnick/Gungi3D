/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Backbuffer.h
 *
 * Summary:	Manages framebuffers for off-screen anti-aliasing (MSAA), HDR, and
 *			post-processing (tone mapping and gamma correction)
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

#ifndef BACKBUFFER_H
#define BACKBUFFER_H

#include "Renderer/Shader.h"

static const std::vector<float> BACKBUFFER_VERTICES
{
	-1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
};

static const float BACKBUFFER_EXPOSURE = 1.0f;
static const float BACKBUFFER_GAMMA = 2.2f;
static const float BACKBUFFER_ALPHA_RATE = 2.0f;

static const GLsizei BACKBUFFER_SAMPLES = 16;

class Backbuffer
{
public:
	// Member functions
	// ----------------
	inline void init() { this->m_alpha = 0.0f; }

	void build(int p_width, int p_height);
	void bind(int p_width, int p_height);

	void resize(int p_width, int p_height);
	void blit(int p_width, int p_height);

	void render(const Shader &p_shader_ref);

	void fade(double p_delta_time);

private:
	void buildSrc(int p_width, int p_height);
	void buildDest(int p_width, int p_height);

	void buildQuad();

	// Member variables
	// ----------------
	float m_alpha = 0.0f;

	// OpenGL handles
	GLuint m_FBO_src, m_tex_src, m_RBO_src;
	GLuint m_FBO_dest, m_tex_dest;

	GLuint m_VAO, m_VBO;
};

#endif // BACKBUFFER_H