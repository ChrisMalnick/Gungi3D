/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Piece.h
 * 
 * Summary:	Extends the model class in order to import, build, and render
 *		a single model with sets of swappable textures that are applied
 *		conditionally
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

#ifndef GFX_PIECE_H
#define GFX_PIECE_H

#include "Game/Piece.h"
#include "World/Model.h"

namespace gfx { class Piece; }

class gfx::Piece : public Model
{
public:
	// Member functions
	// ----------------
	void render(const Shader &p_shader_ref, game::Piece *p_piece_ptr);

	void loadTextures();

private:
	// Member variables
	// ----------------
	// OpenGL handles
	std::vector<GLuint> m_face_texs;
	std::vector<GLuint> m_color_texs;
};

#endif // GFX_PIECE_H
