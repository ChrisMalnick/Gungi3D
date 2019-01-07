/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Shader.cpp
 * 
 * Summary:	Imports and compiles shader modules at runtime that are necessary
 *		to process and render all graphical components
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

#include "Renderer/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Member functions
// ----------------
// Read and build shader program modularly
void Shader::build(const std::string &p_directory_file_root_ref)
{
	this->m_program = glCreateProgram();

	// Shader program will ignore unprovided modules and continue to build
	this->createShaderModule(p_directory_file_root_ref + ".vs", "VERTEX", GL_VERTEX_SHADER);
	this->createShaderModule(p_directory_file_root_ref + ".gs", "GEOMETRY", GL_GEOMETRY_SHADER);
	this->createShaderModule(p_directory_file_root_ref + ".fs", "FRAGMENT", GL_FRAGMENT_SHADER);

	glLinkProgram(this->m_program);

	this->checkLinkErrors(p_directory_file_root_ref, this->m_program);
}

// Create shader module by compiling provided code
void Shader::createShaderModule(const std::string &p_path_ref, const std::string &p_type_ref, GLenum p_shader_type)
{
	std::string shader_module_source = this->loadShaderModule(p_path_ref, p_type_ref);

	if (!shader_module_source.empty())
	{
		GLuint shader_module = glCreateShader(p_shader_type);

		if (shader_module == NULL)
		{
			std::cerr << "ERROR::SHADER::" << p_type_ref << "::CREATE_FAILED >> " << p_path_ref << std::endl;
			return;
		}

		const GLchar *source_code_ptr = shader_module_source.c_str();

		glShaderSource(shader_module, 1, &source_code_ptr, NULL);
		glCompileShader(shader_module);

		this->checkCompileErrors(p_path_ref, p_type_ref, shader_module);

		this->m_modules.push_back(shader_module);

		glAttachShader(this->m_program, shader_module);
	}
}

// Load shader module file contents into string
std::string Shader::loadShaderModule(const std::string &p_path_ref, const std::string &p_type_ref)
{
	std::ifstream ifs;
	std::ostringstream oss;

	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		ifs.open(p_path_ref);
		oss << ifs.rdbuf();
		ifs.close();
	}

	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::SHADER::" << p_type_ref << "::LOAD_FAILED >> " << p_path_ref << std::endl;
		std::cerr << e.what() << std::endl;
	}

	return oss.str();
}

// Check for shader compilation errors
void Shader::checkCompileErrors(const std::string &p_path_ref, const std::string &p_type_ref, GLuint p_shader_module)
{
	GLint success;
	GLchar info_log[1024];

	glGetShaderiv(p_shader_module, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(p_shader_module, 1024, NULL, info_log);

		std::cerr << "ERROR::SHADER::" << p_type_ref << "::COMPILE_FAILED >> " << p_path_ref << std::endl;
		std::cerr << info_log << std::endl;
	}
}

// Check for shader linking errors
void Shader::checkLinkErrors(const std::string &p_directory_file_root_ref, GLuint p_shader_program)
{
	GLint success;
	GLchar info_log[1024];
	
	glGetProgramiv(p_shader_program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(p_shader_program, 1024, NULL, info_log);

		std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED >> " << p_directory_file_root_ref << std::endl;
		std::cerr << info_log << std::endl;
	}
}
