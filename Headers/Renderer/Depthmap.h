/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Depthmap.h
 * 
 * Summary:	Manages framebuffers for storing and sampling depth values of the
 *		scene relative to the light's position and perspective
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

#ifndef DEPTHMAP_H
#define DEPTHMAP_H

#include "Renderer/Shader.h"

static const int DEPTHMAP_CUBEMAP_SIZE = 4096;
static const int DEPTHMAP_2D_SIZE = 16384;

static const float DEPTHMAP_NEAR_PLANE = 1.0f;
static const float DEPTHMAP_FAR_PLANE = 25.0f;

class Depthmap
{
public:
	// Member functions
	// ----------------
	void build();

	void bindCubemap();
	void bind2D();

	void bind(const Shader &p_shader_ref);

	void setUniformsCubemap(const Shader &p_shader_ref);
	void setUniforms2D(const Shader &p_shader_ref);

private:
	void buildCubemap();
	void build2D();

	void setTransforms();

	// Member variables
	// ----------------
	std::vector<glm::mat4> m_transforms;

	// OpenGL handles
	GLuint m_FBO_cubemap, m_tex_cubemap;
	GLuint m_FBO_2D, m_tex_2D;
};

#endif // DEPTHMAP_H
