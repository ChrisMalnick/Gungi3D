/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Depthmap.cpp
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

#include "Renderer/Depthmap.h"
#include "World/Light.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

// Member functions
// ----------------
void Depthmap::build()
{
	buildCubemap();
	build2D();

	setTransforms();
}

void Depthmap::bindCubemap()
{
	glViewport(0, 0, DEPTHMAP_CUBEMAP_SIZE, DEPTHMAP_CUBEMAP_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_cubemap);
}

void Depthmap::bind2D()
{
	glViewport(0, 0, DEPTHMAP_2D_SIZE, DEPTHMAP_2D_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_2D);
}

void Depthmap::bind(const Shader &p_shader_ref)
{
	p_shader_ref.setMat4("u_transform", this->m_transforms[3]);
	p_shader_ref.setFloat("u_far_plane", DEPTHMAP_FAR_PLANE);

	p_shader_ref.setInt("u_depthmap.sampler_cube", 30);
	p_shader_ref.setInt("u_depthmap.sampler_2D", 31);

	glActiveTexture(GL_TEXTURE30);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_tex_cubemap);

	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, this->m_tex_2D);
}

void Depthmap::setUniformsCubemap(const Shader &p_shader_ref)
{
	for (int i = 0; i < 6; ++i)
		p_shader_ref.setMat4("u_transforms[" + std::to_string(i) + "]", this->m_transforms[i]);

	p_shader_ref.setVec3("u_light_pos", LIGHT_POSITION);
	p_shader_ref.setFloat("u_far_plane", DEPTHMAP_FAR_PLANE);
}

void Depthmap::setUniforms2D(const Shader &p_shader_ref)
{
	p_shader_ref.setMat4("u_transform", this->m_transforms[3]);
	p_shader_ref.setVec3("u_light_pos", LIGHT_POSITION);
	p_shader_ref.setFloat("u_far_plane", DEPTHMAP_FAR_PLANE);
}

void Depthmap::buildCubemap()
{
	// Configure tex
	// -------------
	glGenTextures(1, &this->m_tex_cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_tex_cubemap);

	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, DEPTHMAP_CUBEMAP_SIZE, DEPTHMAP_CUBEMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Filtering parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrapping parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure FBO
	// -------------
	glGenFramebuffers(1, &this->m_FBO_cubemap);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_cubemap);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->m_tex_cubemap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::DEPTHMAP::FRAMEBUFFER::CUBEMAP::BUILD_FAILED" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Depthmap::build2D()
{
	// Configure tex
	// -------------
	glGenTextures(1, &this->m_tex_2D);
	glBindTexture(GL_TEXTURE_2D, this->m_tex_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, DEPTHMAP_2D_SIZE, DEPTHMAP_2D_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure FBO
	// -------------
	glGenFramebuffers(1, &this->m_FBO_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_2D);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->m_tex_2D, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::DEPTHMAP::FRAMEBUFFER::2D::BUILD_FAILED" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Depthmap::setTransforms()
{
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, DEPTHMAP_NEAR_PLANE, DEPTHMAP_FAR_PLANE);

	this->m_transforms.push_back(projection * glm::lookAt(LIGHT_POSITION, LIGHT_POSITION + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3( 0.0f, -1.0f,  0.0f))); // x+ (right)
	this->m_transforms.push_back(projection * glm::lookAt(LIGHT_POSITION, LIGHT_POSITION + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3( 0.0f, -1.0f,  0.0f))); // x- (left)

	this->m_transforms.push_back(projection * glm::lookAt(LIGHT_POSITION, LIGHT_POSITION + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3( 0.0f,  0.0f,  1.0f))); // y+ (top)
	this->m_transforms.push_back(projection * glm::lookAt(LIGHT_POSITION, LIGHT_POSITION + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3( 0.0f,  0.0f, -1.0f))); // y- (bottom)

	this->m_transforms.push_back(projection * glm::lookAt(LIGHT_POSITION, LIGHT_POSITION + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3( 0.0f, -1.0f,  0.0f))); // z+ (back)
	this->m_transforms.push_back(projection * glm::lookAt(LIGHT_POSITION, LIGHT_POSITION + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3( 0.0f, -1.0f,  0.0f))); // z- (front)
}
