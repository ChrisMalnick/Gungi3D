/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Backbuffer.cpp
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

#include "Renderer/Backbuffer.h"

#include <iostream>

// Member functions
// ----------------
void Backbuffer::build(int p_width, int p_height)
{
	this->buildSrc(p_width, p_height);
	this->buildDest(p_width, p_height);

	this->buildQuad();
}

void Backbuffer::bind(int p_width, int p_height)
{
	glViewport(0, 0, p_width, p_height);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_src);
}

void Backbuffer::resize(int p_width, int p_height)
{
	// Src tex
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->m_tex_src);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, BACKBUFFER_SAMPLES, GL_RGBA16F, p_width, p_height, GL_TRUE);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// Src RBO
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_RBO_src);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, BACKBUFFER_SAMPLES, GL_DEPTH_COMPONENT32F, p_width, p_height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Dest tex
	glBindTexture(GL_TEXTURE_2D, this->m_tex_dest);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Backbuffer::blit(int p_width, int p_height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->m_FBO_src);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->m_FBO_dest);

	glBlitFramebuffer(0, 0, p_width, p_height, 0, 0, p_width, p_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Backbuffer::render(const Shader &p_shader_ref)
{
	p_shader_ref.setFloat("u_exposure", BACKBUFFER_EXPOSURE);
	p_shader_ref.setFloat("u_gamma", BACKBUFFER_GAMMA);
	p_shader_ref.setFloat("u_alpha", this->m_alpha);

	glBindTexture(GL_TEXTURE_2D, this->m_tex_dest);

	glBindVertexArray(this->m_VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Backbuffer::fade(double p_delta_time)
{
	this->m_alpha += static_cast<float>(p_delta_time) * BACKBUFFER_ALPHA_RATE;

	if (this->m_alpha > 1.0f)
		this->m_alpha = 1.0f;
}

void Backbuffer::buildSrc(int p_width, int p_height)
{
	// Configure tex
	// -------------
	glGenTextures(1, &this->m_tex_src);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->m_tex_src);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, BACKBUFFER_SAMPLES, GL_RGBA16F, p_width, p_height, GL_TRUE);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// Configure RBO
	// -------------
	glGenRenderbuffers(1, &this->m_RBO_src);
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_RBO_src);

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, BACKBUFFER_SAMPLES, GL_DEPTH_COMPONENT32F, p_width, p_height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Configure FBO
	// -------------
	glGenFramebuffers(1, &this->m_FBO_src);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_src);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, this->m_tex_src, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->m_RBO_src);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::BACKBUFFER::FRAMEBUFFER::SRC::BUILD_FAILED" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Backbuffer::buildDest(int p_width, int p_height)
{
	// Configure tex
	// -------------
	glGenTextures(1, &this->m_tex_dest);
	glBindTexture(GL_TEXTURE_2D, this->m_tex_dest);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, NULL);

	// Filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Configure FBO
	// -------------
	glGenFramebuffers(1, &this->m_FBO_dest);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO_dest);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_tex_dest, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR::BACKBUFFER::FRAMEBUFFER::DEST::BUILD_FAILED" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Backbuffer::buildQuad()
{
	glGenVertexArrays(1, &this->m_VAO);
	glBindVertexArray(this->m_VAO);

	glGenBuffers(1, &this->m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);

	glBufferData(GL_ARRAY_BUFFER, BACKBUFFER_VERTICES.size() * sizeof(float), &BACKBUFFER_VERTICES[0], GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}