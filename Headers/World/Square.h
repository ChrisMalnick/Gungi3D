/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Square.h
 *
 * Summary:	Builds and renders an internal mesh of a quad of varying color used
 *			to indicate the various potential interaction available to the user
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

#ifndef GFX_SQUARE_H
#define GFX_SQUARE_H

#include "Renderer/Shader.h"

static const std::vector<float> SQUARE_VERTICES
{
	-0.975f,  0.0f, -0.975f,
	 0.975f,  0.0f, -0.975f,
	-0.975f,  0.0f,  0.975f,
	 0.975f,  0.0f,  0.975f,
};

namespace gfx { class Square; }

class gfx::Square
{
public:
	// Member functions
	// ----------------
	void build();
	void render(const Shader &p_shader_ref, glm::vec4 p_color);

private:
	// Member variables
	// ----------------
	// OpenGL handles
	GLuint m_VAO, m_VBO;
};

#endif // GFX_SQUARE_H