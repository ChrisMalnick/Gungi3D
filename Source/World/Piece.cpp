/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Piece.cpp
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

#include "World/Piece.h"

// Member functions
// ----------------
void gfx::Piece::render(const Shader &p_shader_ref, game::Piece *p_piece_ptr)
{
	this->m_meshes[0].render(p_shader_ref, this->m_face_texs[p_piece_ptr->getFront()]);
	this->m_meshes[1].render(p_shader_ref, (p_piece_ptr->getFront() == game::Piece::COMMANDER ? this->m_color_texs[p_piece_ptr->getColor() == game::Piece::WHITE] : this->m_face_texs[p_piece_ptr->getBack() + (p_piece_ptr->getColor() == game::Piece::WHITE ? 10 : 0)]));
	this->m_meshes[2].render(p_shader_ref, this->m_color_texs[p_piece_ptr->getColor() == game::Piece::WHITE]);
}

void gfx::Piece::loadTextures()
{
	this->m_face_texs.push_back(NULL); // Index 0 is empty since face indexed at 0 is blank

	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Commander.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Captain.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Samurai.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Spy.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Catapult.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Fortress.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Hidden_Dragon.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Prodigy.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Bow.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Front/Pawn.png"));

	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Pistol.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Pike.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Clandestinite.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Lance.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Dragon_King.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Phoenix.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Arrow.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Bronze.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Silver.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/Black/Gold.png"));

	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Pistol.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Pike.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Clandestinite.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Lance.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Dragon_King.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Phoenix.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Arrow.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Bronze.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Silver.png"));
	this->m_face_texs.push_back(this->loadTexture("./Resources/Textures/Faces/Back/White/Gold.png"));

	this->m_color_texs.push_back(this->loadTexture("./Resources/Textures/Black.png"));
	this->m_color_texs.push_back(this->loadTexture("./Resources/Textures/White.png"));
}
