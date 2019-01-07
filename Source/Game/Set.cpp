/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Set.cpp
 * 
 * Summary:	Builds the set of all game materials that are referenced by the
 *		game containers in order to represent the current state of the game
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

#include "Game/Set.h"

// Class functions
// ---------------
// Constructor
Set::Set()
{
	// Build pieces
	// ------------
	this->buildPieces(game::Piece::BLACK);
	this->buildPieces(game::Piece::WHITE);

	// Build squares
	// -------------
	for (int i = 0; i < NUM_SQUARES; ++i)
	{
		if (i >= game::Square::BOARD)
			this->m_squares[i].build(i, game::Square::BOARD);

		else if (i >= game::Square::WHITE_HAND)
			this->m_squares[i].build(i, game::Square::WHITE_HAND);

		else if (i >= game::Square::BLACK_HAND)
			this->m_squares[i].build(i, game::Square::BLACK_HAND);
	}
}

// Member functions
// ----------------
void Set::init()
{
	for (auto &elem : this->m_pieces)
		elem.init();

	for (auto &elem : this->m_squares)
		elem.init();
}

void Set::buildPieces(game::Piece::Color p_color)
{
	this->m_pieces[0 + p_color].build(0 + p_color, game::Piece::COMMANDER, game::Piece::BLANK, p_color);

	this->m_pieces[1 + p_color].build(1 + p_color, game::Piece::CAPTAIN, game::Piece::PISTOL, p_color);
	this->m_pieces[2 + p_color].build(2 + p_color, game::Piece::CAPTAIN, game::Piece::PISTOL, p_color);

	this->m_pieces[3 + p_color].build(3 + p_color, game::Piece::SAMURAI, game::Piece::PIKE, p_color);
	this->m_pieces[4 + p_color].build(4 + p_color, game::Piece::SAMURAI, game::Piece::PIKE, p_color);

	this->m_pieces[5 + p_color].build(5 + p_color, game::Piece::SPY, game::Piece::CLANDESTINITE, p_color);
	this->m_pieces[6 + p_color].build(6 + p_color, game::Piece::SPY, game::Piece::CLANDESTINITE, p_color);
	this->m_pieces[7 + p_color].build(7 + p_color, game::Piece::SPY, game::Piece::CLANDESTINITE, p_color);

	this->m_pieces[8 + p_color].build(8 + p_color, game::Piece::CATAPULT, game::Piece::LANCE, p_color);

	this->m_pieces[9 + p_color].build(9 + p_color, game::Piece::FORTRESS, game::Piece::LANCE, p_color);

	this->m_pieces[10 + p_color].build(10 + p_color, game::Piece::HIDDEN_DRAGON, game::Piece::DRAGON_KING, p_color);

	this->m_pieces[11 + p_color].build(11 + p_color, game::Piece::PRODIGY, game::Piece::PHOENIX, p_color);

	this->m_pieces[12 + p_color].build(12 + p_color, game::Piece::BOW, game::Piece::ARROW, p_color);
	this->m_pieces[13 + p_color].build(13 + p_color, game::Piece::BOW, game::Piece::ARROW, p_color);

	this->m_pieces[14 + p_color].build(14 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);
	this->m_pieces[15 + p_color].build(15 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);
	this->m_pieces[16 + p_color].build(16 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);
	this->m_pieces[17 + p_color].build(17 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);
	this->m_pieces[18 + p_color].build(18 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);
	this->m_pieces[19 + p_color].build(19 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);
	this->m_pieces[20 + p_color].build(20 + p_color, game::Piece::PAWN, game::Piece::BRONZE, p_color);

	this->m_pieces[21 + p_color].build(21 + p_color, game::Piece::PAWN, game::Piece::SILVER, p_color);

	this->m_pieces[22 + p_color].build(22 + p_color, game::Piece::PAWN, game::Piece::GOLD, p_color);
}
