/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Picture.cpp
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

#include "World/Picture.h"

extern int rand(int p_min, int p_max);

// Member functions
// ----------------
void Picture::init()
{
	this->m_indices[0] = rand(0, this->m_texs.size() - 1);

	do this->m_indices[1] = rand(0, this->m_texs.size() - 1);
	while (this->m_indices[1] == this->m_indices[0]);
	
	do this->m_indices[2] = rand(0, this->m_texs.size() - 1);
	while (this->m_indices[2] == this->m_indices[0] || this->m_indices[2] == this->m_indices[1]);

	do this->m_indices[3] = rand(0, this->m_texs.size() - 1);
	while (this->m_indices[3] == this->m_indices[0] || this->m_indices[3] == this->m_indices[1] || this->m_indices[3] == this->m_indices[2]);
}

void Picture::render(const Shader &p_shader_ref, int p_index)
{
	this->m_meshes[0].render(p_shader_ref, this->m_texs[this->m_indices[p_index]]);
	this->m_meshes[1].render(p_shader_ref);
}

void Picture::loadTextures(GLint p_wrap_param)
{
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/1.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/2.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/3.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/4.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/5.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/6.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/7.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/8.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/9.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/10.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/11.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/12.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/13.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/14.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/15.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/16.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/17.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/18.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/19.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/20.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/21.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/22.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/23.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/24.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/25.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/26.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/27.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/28.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/29.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/30.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/31.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/32.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/33.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/34.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/35.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/36.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/37.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/38.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/39.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/40.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/41.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/42.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/43.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/44.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/45.png", p_wrap_param));
	this->m_texs.push_back(this->loadTexture("./Resources/Textures/Prints/46.png", p_wrap_param));
}