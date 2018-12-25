/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Model.cpp
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "World/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>
#include <unordered_map>

struct Edge
{
	GLuint index_a;
	GLuint index_b;

	bool operator==(const Edge &p_edge_ref) const
	{
		return (p_edge_ref.index_a == index_a && p_edge_ref.index_b == index_b);
	}
};

struct Hash
{
	std::size_t operator()(const Edge &p_edge_ref) const
	{
		return (std::hash<GLuint>()(p_edge_ref.index_a) ^ std::hash<GLuint>()(p_edge_ref.index_b));
	}
};

extern void drawLoadScreen(const std::string &p_path_ref, void *p_window_ptr = nullptr);

// Member functions
// ----------------
void Model::build(const std::string &p_path_ref)
{
	drawLoadScreen(p_path_ref);
	
	Assimp::Importer importer;
	const aiScene *scene_ptr = importer.ReadFile(p_path_ref, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene_ptr || scene_ptr->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_ptr->mRootNode)
	{
		std::cerr << "ERROR::MODEL::ASSIMP::SCENE::LOAD_FAILED >> " << p_path_ref << std::endl;
		std::cerr << importer.GetErrorString() << std::endl;

		return;
	}

	this->processNode(scene_ptr->mRootNode, scene_ptr);
}

void Model::render(const Shader &p_shader_ref)
{
	for (auto &elem : this->m_meshes)
		elem.render(p_shader_ref);
}

void Model::processNode(aiNode *p_node_ptr, const aiScene *p_scene_ptr)
{
	// Process node's meshes
	for (unsigned int i = 0; i < p_node_ptr->mNumMeshes; ++i)
		this->processMesh(p_scene_ptr->mMeshes[p_node_ptr->mMeshes[i]], p_scene_ptr);

	// Process node's children
	for (unsigned int i = 0; i < p_node_ptr->mNumChildren; ++i)
		this->processNode(p_node_ptr->mChildren[i], p_scene_ptr);
}

void Model::processMesh(aiMesh *p_mesh_ptr, const aiScene *p_scene_ptr)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Process mesh's vertices
	for (unsigned int i = 0; i < p_mesh_ptr->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex.position = glm::vec3(p_mesh_ptr->mVertices[i].x, p_mesh_ptr->mVertices[i].y, p_mesh_ptr->mVertices[i].z);
		vertex.UV = (p_mesh_ptr->mTextureCoords[0] ? glm::vec2(p_mesh_ptr->mTextureCoords[0][i].x, p_mesh_ptr->mTextureCoords[0][i].y) : glm::vec2(0.0f));
		vertex.normal = glm::vec3(p_mesh_ptr->mNormals[i].x, p_mesh_ptr->mNormals[i].y, p_mesh_ptr->mNormals[i].z);
		vertex.tangent = glm::vec3(p_mesh_ptr->mTangents[i].x, p_mesh_ptr->mTangents[i].y, p_mesh_ptr->mTangents[i].z);
		vertex.bitangent = glm::vec3(p_mesh_ptr->mBitangents[i].x, p_mesh_ptr->mBitangents[i].y, p_mesh_ptr->mBitangents[i].z);

		vertices.push_back(vertex);
	}

	// Process mesh's indices
	this->processIndices(p_mesh_ptr, indices);

	// Process mesh's materials
	if (p_mesh_ptr->mMaterialIndex >= 0)
	{
		aiMaterial* material_ptr = p_scene_ptr->mMaterials[p_mesh_ptr->mMaterialIndex];

		std::vector<Texture> diffuse_maps = this->loadMaterials(material_ptr, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		std::vector<Texture> specular_maps = this->loadMaterials(material_ptr, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

		std::vector<Texture> normal_maps = this->loadMaterials(material_ptr, aiTextureType_HEIGHT, "normal");
		textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

		std::vector<Texture> height_maps = this->loadMaterials(material_ptr, aiTextureType_AMBIENT, "height");
		textures.insert(textures.end(), height_maps.begin(), height_maps.end());
	}

	this->m_meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::processIndices(aiMesh *p_mesh_ptr, std::vector<GLuint> &p_indices_ref)
{
	for (unsigned int i = 0; i < p_mesh_ptr->mNumFaces; ++i)
	{
		for (unsigned int j = 0; j < p_mesh_ptr->mFaces[i].mNumIndices; ++j)
			p_indices_ref.push_back(p_mesh_ptr->mFaces[i].mIndices[j]);
	}
}

void Model::processAdjacencies(aiMesh *p_mesh_ptr, std::vector<GLuint> &p_indices_ref)
{
	std::unordered_map<Edge, GLuint, Hash> index_map;

	for (unsigned int i = 0; i < p_mesh_ptr->mNumFaces; ++i)
	{
		aiFace &face_ref = p_mesh_ptr->mFaces[i];

		GLuint index1 = face_ref.mIndices[0];
		GLuint index2 = face_ref.mIndices[1];
		GLuint index3 = face_ref.mIndices[2];

		Edge edge1 = { index1, index2 };
		Edge edge2 = { index2, index3 };
		Edge edge3 = { index3, index1 };

		index_map[edge1] = index3;
		index_map[edge2] = index1;
		index_map[edge3] = index2;
	}

	for (unsigned int i = 0; i < p_mesh_ptr->mNumFaces; ++i)
	{
		aiFace &face_ref = p_mesh_ptr->mFaces[i];

		GLuint index1 = face_ref.mIndices[0];
		GLuint index2 = face_ref.mIndices[1];
		GLuint index3 = face_ref.mIndices[2];

		Edge edge1 = { index2, index1 };
		Edge edge2 = { index3, index2 };
		Edge edge3 = { index1, index3 };

		p_indices_ref.push_back(index1);
		p_indices_ref.push_back(index_map[edge1]);
		p_indices_ref.push_back(index2);
		p_indices_ref.push_back(index_map[edge2]);
		p_indices_ref.push_back(index3);
		p_indices_ref.push_back(index_map[edge3]);
	}
}

std::vector<Texture> Model::loadMaterials(aiMaterial *p_material_ptr, aiTextureType p_texture_type, const std::string &p_type_ref)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < p_material_ptr->GetTextureCount(p_texture_type); ++i)
	{
		aiString string;
		p_material_ptr->GetTexture(p_texture_type, i, &string);

		std::string path = std::string(string.C_Str());
		
		bool skip = false;

		for (auto &elem : this->m_textures)
		{
			if (elem.path == path)
			{
				textures.push_back(elem);
				skip = true;

				break;
			}
		}

		if (!skip)
		{
			Texture texture;

			texture.tex = this->loadTexture(path);
			texture.type = p_texture_type;
			texture.path = path;

			textures.push_back(texture);
			this->m_textures.push_back(texture);
		}
	}

	return textures;
}

void Model::format(GLenum &p_internal_format_ref, GLenum &p_format_ref, int p_num_components, bool p_linearize)
{
	switch (p_num_components)
	{
	case 1:
		p_internal_format_ref = p_format_ref = GL_RED;
		return;

	case 2:
		p_internal_format_ref = p_format_ref = GL_RG;
		return;

	case 3:
		p_internal_format_ref = (p_linearize ? GL_SRGB : GL_RGB);
		p_format_ref = GL_RGB;

		return;

	case 4:
		p_internal_format_ref = (p_linearize ? GL_SRGB_ALPHA : GL_RGBA);
		p_format_ref = GL_RGBA;

		return;
	}
}

GLuint Model::loadTexture(const std::string &p_path_ref, GLint p_wrap_param)
{
	drawLoadScreen(p_path_ref);
	
	int width, height, num_components;
	unsigned char *data_ptr = stbi_load(p_path_ref.c_str(), &width, &height, &num_components, 0);

	if (data_ptr == NULL)
	{
		std::cerr << "ERROR::MODEL::TEXTURE::LOAD_FAILED >> " << p_path_ref << std::endl;
		return NULL;
	}

	// Configure tex
	// -------------
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	GLenum internal_format, format;
	this->format(internal_format, format, num_components, true);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data_ptr);

	// Filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, p_wrap_param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, p_wrap_param);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data_ptr);

	return tex;
}