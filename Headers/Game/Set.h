/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Set.h
 *
 * Summary:	Builds the set of all game materials that are referenced by the
 *			game containers in order to represent the current state of the game
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

#ifndef SET_H
#define SET_H

#include "Game\Piece.h"
#include "Game\Square.h"

#define NUM_PIECES 46
#define NUM_SQUARES 129

class Set
{
public:
	// Class functions
	// ---------------
	// Constructor
	Set();

	// Member functions
	// ----------------
	inline game::Piece* getPiecePtr(int p_index) { return &this->m_pieces[p_index]; }
	inline game::Square* getSquarePtr(int p_index) { return &this->m_squares[p_index]; }

	void init();

private:
	void buildPieces(game::Piece::Color p_color);

	// Member variables
	// ----------------
	game::Piece m_pieces[NUM_PIECES];
	game::Square m_squares[NUM_SQUARES];
};

#endif // SET_H