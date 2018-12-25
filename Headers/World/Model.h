/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Model.h
 *
 * Summary:	Imports, processes, and stores external geometric and color data in
 *			the form of polygon meshes and textures
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

#ifndef MODEL_H
#define MODEL_H

#include "World/Mesh.h"

#include <assimp/scene.h>

class Model
{
public:
	// Member functions
	// ----------------
	void build(const std::string &p_path_ref);
	void render(const Shader &p_shader_ref);

private:
	void processNode(aiNode *p_node_ptr, const aiScene *p_scene_ptr);
	void processMesh(aiMesh *p_mesh_ptr, const aiScene *p_scene_ptr);

	void processIndices(aiMesh *p_mesh_ptr, std::vector<GLuint> &p_indices_ref);
	void processAdjacencies(aiMesh *p_mesh_ptr, std::vector<GLuint> &p_indices_ref);

	std::vector<Texture> loadMaterials(aiMaterial *p_material_ptr, aiTextureType p_texture_type, const std::string &p_name_ref);

	void format(GLenum &p_internal_format_ref, GLenum &p_format_ref, int p_num_components, bool p_linearize);

protected:
	GLuint loadTexture(const std::string &p_path_ref, GLint p_wrap_param = GL_REPEAT);

	// Member variables
	// ----------------
	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_textures;
};

#endif // MODEL_H