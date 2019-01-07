/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Shader.h
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

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // For all OpenGL headers
#include <glm/glm.hpp>

#include <vector>

class Shader
{
public:
	// Member functions
	// ----------------
	inline void bind() { glUseProgram(this->m_program); } // Activate shader program

	// Query uniform location and set value
	inline void setBool(const std::string &p_name_ref, bool p_value) const { glUniform1i(glGetUniformLocation(this->m_program, p_name_ref.c_str()), p_value); }
	inline void setInt(const std::string &p_name_ref, int p_value) const { glUniform1i(glGetUniformLocation(this->m_program, p_name_ref.c_str()), p_value); }
	inline void setFloat(const std::string &p_name_ref, float p_value) const { glUniform1f(glGetUniformLocation(this->m_program, p_name_ref.c_str()), p_value); }

	inline void setVec2(const std::string &p_name_ref, const glm::vec2 &p_vec_ref) const { glUniform2fv(glGetUniformLocation(this->m_program, p_name_ref.c_str()), 1, &p_vec_ref[0]); }
	inline void setVec2(const std::string &p_name_ref, float x, float y) const { glUniform2f(glGetUniformLocation(this->m_program, p_name_ref.c_str()), x, y); }

	inline void setVec3(const std::string &p_name_ref, const glm::vec3 &p_vec_ref) const { glUniform3fv(glGetUniformLocation(this->m_program, p_name_ref.c_str()), 1, &p_vec_ref[0]); }
	inline void setVec3(const std::string &p_name_ref, float x, float y, float z) const { glUniform3f(glGetUniformLocation(this->m_program, p_name_ref.c_str()), x, y, z); }

	inline void setVec4(const std::string &p_name_ref, const glm::vec4 &p_vec_ref) const { glUniform4fv(glGetUniformLocation(this->m_program, p_name_ref.c_str()), 1, &p_vec_ref[0]); }
	inline void setVec4(const std::string &p_name_ref, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(this->m_program, p_name_ref.c_str()), x, y, z, w); }

	inline void setMat2(const std::string &p_name_ref, const glm::mat2 &p_mat_ref) const { glUniformMatrix2fv(glGetUniformLocation(this->m_program, p_name_ref.c_str()), 1, GL_FALSE, &p_mat_ref[0][0]); }
	inline void setMat3(const std::string &p_name_ref, const glm::mat3 &p_mat_ref) const { glUniformMatrix3fv(glGetUniformLocation(this->m_program, p_name_ref.c_str()), 1, GL_FALSE, &p_mat_ref[0][0]); }
	inline void setMat4(const std::string &p_name_ref, const glm::mat4 &p_mat_ref) const { glUniformMatrix4fv(glGetUniformLocation(this->m_program, p_name_ref.c_str()), 1, GL_FALSE, &p_mat_ref[0][0]); }

	void build(const std::string &p_directory_file_root_ref); // Read and build shader program modularly

private:
	void createShaderModule(const std::string &p_path_ref, const std::string &p_type_ref, GLenum p_shader_type); // Create shader module by compiling provided code

	std::string loadShaderModule(const std::string &p_path_ref, const std::string &p_type_ref); // Load shader module file contents into string

	void checkCompileErrors(const std::string &p_path_ref, const std::string &p_type_ref, GLuint p_shader_module); // Check for shader compilation errors
	void checkLinkErrors(const std::string &p_directory_file_root_ref, GLuint p_shader_program); // Check for shader linking errors

	// Member variables
	// ----------------
	// OpenGL handles
	GLuint m_program;
	std::vector<GLuint> m_modules;
};

#endif // SHADER_H
