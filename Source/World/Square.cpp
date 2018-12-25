/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Square.cpp
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

#include "World/Square.h"

// Member functions
// ----------------
void gfx::Square::build()
{
	glGenVertexArrays(1, &this->m_VAO);
	glBindVertexArray(this->m_VAO);

	glGenBuffers(1, &this->m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);

	glBufferData(GL_ARRAY_BUFFER, SQUARE_VERTICES.size() * sizeof(float), &SQUARE_VERTICES[0], GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gfx::Square::render(const Shader &p_shader_ref, glm::vec4 p_color)
{
	p_shader_ref.setVec4("u_overlay.color", p_color);

	glBindVertexArray(this->m_VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}