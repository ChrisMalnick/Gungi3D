/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Picture.h
 *
 * Summary:	Extends the model class in order to import, build, and render
 *			a single model with a set of swappable textures that are applied
 *			pseudorandomly
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

#ifndef PICTURE_H
#define PICTURE_H

#include "World/Model.h"

class Picture : public Model
{
public:
	// Member functions
	// ----------------
	void init();
	void render(const Shader &p_shader_ref, int p_index);

	void loadTextures(GLint p_wrap_param = GL_CLAMP_TO_EDGE);

private:
	// Member variables
	// ----------------
	int m_indices[4];

	// OpenGL handles
	std::vector<GLuint> m_texs;
};

#endif // PICTURE_H