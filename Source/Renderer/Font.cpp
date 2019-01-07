/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Font.cpp
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

#include "Renderer/Font.h"

#include <iostream>

// Member functions
// ----------------
void Font::init(const std::string &p_path_ref, FT_Long p_index)
{
	if (FT_Init_FreeType(&this->m_lib))
		std::cerr << "ERROR::FONT::FREETYPE::LIBRARY::INITIALIZE_FAILED" << std::endl;

	if (FT_New_Face(this->m_lib, p_path_ref.c_str(), p_index, &this->m_face))
		std::cerr << "ERROR::FONT::FREETYPE::FACE::OPEN_FAILED >> " << p_path_ref.c_str() << std::endl;

	if (FT_Stroker_New(this->m_lib, &this->m_stroker))
		std::cerr << "ERROR::FONT::FREETYPE::STROKER::CREATE_FAILED" << std::endl;
}

void Font::release()
{
	FT_Stroker_Done(this->m_stroker);
	
	if (FT_Done_Face(this->m_face))
		std::cerr << "ERROR::FONT::FREETYPE::FACE::RELEASE_FAILED" << std::endl;

	if (FT_Done_FreeType(this->m_lib))
		std::cerr << "ERROR::FONT::FREETYPE::LIBRARY::RELEASE_FAILED" << std::endl;
}

void Font::build()
{
	glGenVertexArrays(1, &this->m_VAO);
	glBindVertexArray(this->m_VAO);

	glGenBuffers(1, &this->m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);

	glBufferData(GL_ARRAY_BUFFER, VERTS_PER_QUAD * ATTRIBS_PER_VERT * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW); // Utilize dynamic draw due to frequent updates to VBO contents

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, ATTRIBS_PER_VERT * sizeof(GLfloat), (void*)0);

	// UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, ATTRIBS_PER_VERT * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Font::render(const std::string &p_text_ref, glm::vec2 p_pos, GLfloat p_scale, bool p_stroke)
{
	glBindVertexArray(this->m_VAO);

	// Iterate through all characters of text
	std::string::const_iterator c;

	for (c = p_text_ref.begin(); c != p_text_ref.end(); ++c)
	{
		Glyph glyph = this->m_glyphs[*c];

		GLfloat xpos = p_pos.x + (p_stroke ? glyph.bearing_stroke.x : glyph.bearing_face.x) * p_scale;
		GLfloat ypos = p_pos.y - (p_stroke ? glyph.size_stroke.y - glyph.bearing_stroke.y : glyph.size_face.y - glyph.bearing_face.y) * p_scale; // Difference between height and top side bearing gives baseline offset (for characters like g, j, p, q, y, etc.)

		GLfloat width = (p_stroke ? glyph.size_stroke.x : glyph.size_face.x) * p_scale;
		GLfloat height = (p_stroke ? glyph.size_stroke.y : glyph.size_face.y) * p_scale;

		// Generate quad based on glyph metrics
		GLfloat quad[VERTS_PER_QUAD][ATTRIBS_PER_VERT] =
		{
			{ xpos,			ypos + height,	0.0f,	0.0f },
			{ xpos,			ypos,			0.0f,	1.0f },
			{ xpos + width,	ypos,			1.0f,	1.0f },

			{ xpos,			ypos + height,	0.0f,	0.0f },
			{ xpos + width,	ypos,			1.0f,	1.0f },
			{ xpos + width,	ypos + height,	1.0f,	0.0f },
		};

		// Render glyph texture onto quad
		glBindTexture(GL_TEXTURE_2D, (p_stroke ? glyph.tex_stroke : glyph.tex_face));

		// Update content of VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, VERTS_PER_QUAD);

		// Advance cursor (in 1/64th pixel intervals)
		p_pos.x += (glyph.advance >> 6) * p_scale; // Bitshift by 6 (2 ^ 6 = 64)

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
}

// Loads set of ASCII characters up to designated byte count (8-bit variant ASCII)
void Font::loadGlyphs(GLubyte p_count)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Set per-pixel byte-aligment equal to 1 (red channel for grayscale)

	for (GLubyte c = 0; c < p_count; ++c)
	{
		// Get character index and load glyph into slot
		if (FT_Load_Char(this->m_face, c, FT_LOAD_DEFAULT))
		{
			std::cerr << "ERROR::FONT::FREETYPE::GLYPH::LOAD_FAILED >> " << c << std::endl;
			continue;
		}

		// Create copy of glyph and generate stroked bitmap
		FT_Glyph glyph_copy;

		FT_Get_Glyph(this->m_face->glyph, &glyph_copy);
		FT_Glyph_StrokeBorder(&glyph_copy, this->m_stroker, false, true);
		FT_Glyph_To_Bitmap(&glyph_copy, FT_RENDER_MODE_NORMAL, nullptr, true);

		FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph_copy);

		// Generate unstroked bitmap
		if (FT_Render_Glyph(this->m_face->glyph, FT_RENDER_MODE_NORMAL))
		{
			std::cerr << "ERROR::FONT::FREETYPE::GLYPH::CONVERT_FAILED >> " << c << std::endl;
			continue;
		}

		// Generate textures
		GLuint tex_face;
		this->loadTexture(tex_face, this->m_face->glyph->bitmap);

		GLuint tex_stroke;
		this->loadTexture(tex_stroke, bitmap_glyph->bitmap);

		// Build and store glyph
		Glyph glyph =
		{
			tex_face,
			tex_stroke,

			glm::ivec2(this->m_face->glyph->bitmap.width, this->m_face->glyph->bitmap.rows),
			glm::ivec2(bitmap_glyph->bitmap.width, bitmap_glyph->bitmap.rows),

			glm::ivec2(this->m_face->glyph->bitmap_left, this->m_face->glyph->bitmap_top),
			glm::ivec2(bitmap_glyph->left, bitmap_glyph->top),

			static_cast<unsigned int>(this->m_face->glyph->advance.x),
		};

		this->m_glyphs.insert(std::pair<GLchar, Glyph>(c, glyph));
	}
}

void Font::loadTexture(GLuint &p_tex_ref, FT_Bitmap p_bitmap)
{
	// Configure tex
	// -------------
	glGenTextures(1, &p_tex_ref);
	glBindTexture(GL_TEXTURE_2D, p_tex_ref);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, p_bitmap.width, p_bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, p_bitmap.buffer);

	// Filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);
}
