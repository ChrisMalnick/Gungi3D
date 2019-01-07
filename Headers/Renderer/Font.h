/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Font.h
 * 
 * Summary:	Handles the loading, building, and rendering of the individual
 *		glyphs contained within provided font files
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

#ifndef FONT_H
#define FONT_H

#include "Renderer/Shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <map>

#define VERTS_PER_QUAD 6 // Two tris per quad
#define ATTRIBS_PER_VERT 4 // (x, y) and (u, v)

struct Glyph
{
	// OpenGL handles
	GLuint tex_face;
	GLuint tex_stroke;

	// Metrics
	glm::ivec2 size_face;
	glm::ivec2 size_stroke;

	glm::ivec2 bearing_face;
	glm::ivec2 bearing_stroke;

	unsigned int advance; // 1/64th pixels
};

class Font
{
public:
	// Member functions
	// ----------------
	inline void setSize(FT_UInt p_width, FT_UInt p_height) { FT_Set_Pixel_Sizes(this->m_face, p_width, p_height); } // Sets font size in pixels (NULL parameter will assume value of other)
	inline void setStroker(FT_Fixed p_thickness, FT_Stroker_LineCap p_line_cap, FT_Stroker_LineJoin p_line_join, FT_Fixed p_miter_limit) { FT_Stroker_Set(this->m_stroker, p_thickness * 64, p_line_cap, p_line_join, p_miter_limit); }

	void init(const std::string &p_path_ref, FT_Long p_index);
	void release();

	void build();
	void render(const std::string &p_text_ref, glm::vec2 p_pos, GLfloat p_scale, bool p_stroke);

	void loadGlyphs(GLubyte p_count); // Loads set of ASCII characters up to designated byte count (8-bit variant ASCII)
	void loadTexture(GLuint &p_tex_ref, FT_Bitmap p_bitmap);

private:
	// Member variables
	// ----------------
	std::map<GLchar, Glyph> m_glyphs;

	// FreeType handles
	FT_Library m_lib;

	FT_Face m_face;
	FT_Stroker m_stroker;

	// OpenGL handles
	GLuint m_VAO, m_VBO;
};

#endif // FONT_H
