/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Mesh.h
 * 
 * Summary:	Builds and renders the individual components of models as separate
 *		sets of vertex, index, and color data
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

#ifndef MESH_H
#define MESH_H

#include "Renderer/Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec2 UV;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Texture
{
	GLuint tex;

	std::string type;
	std::string path;
};

class Mesh
{
public:
	// Class functions
	// ---------------
	// Constructor
	Mesh(std::vector<Vertex> p_vertices, std::vector<GLuint> p_indices, std::vector<Texture> p_textures);

	// Member functions
	// ----------------
	void render(const Shader &p_shader_ref);
	void render(const Shader &p_shader_ref, GLuint p_texture);

private:
	void build();

	// Member variables
	// ----------------
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<Texture> m_textures;

	// OpenGL handles
	GLuint m_VAO, m_VBO, m_EBO;
};

#endif // MESH_H
