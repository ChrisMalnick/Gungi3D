/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	MRE.h
 * 
 * Summary:	Maintains a logical representation of the game board's spaces with
 *		respect to Mobile Range Expansion
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

#ifndef MRE_H
#define MRE_H

#include "Game/Piece.h"

#define BOARD_ROWS 9
#define BOARD_COLS 9

#define BLACK_TERRITORY 6
#define WHITE_TERRITORY 2

struct Range
{
	game::Piece::Face face;
	game::Piece::Color color;
};

struct Coords2D
{
	int x;
	int y;
};

class MRE
{
public:
	// Member functions
	// ----------------
	inline std::vector<Range>& getRangeRef(int x, int y) { return this->m_AOE[x][y]; }

	void setRange(game::Piece *p_piece_ptr, int x, int y);

	void removeRange(game::Piece *p_piece_ptr);

	bool inRange(game::Piece::Color p_alignment, int x, int y);

	bool contains(game::Piece *p_piece_ptr, int x, int y);

private:
	inline int getLowerBound(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? LOWER_BOUND : BLACK_TERRITORY); }
	inline int getUpperBound(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? WHITE_TERRITORY : UPPER_BOUND); }

	std::vector<Coords2D> getCatapultRange(int x, int y);

	// Member variables
	// ----------------
	std::vector<Range> m_AOE[BOARD_COLS][BOARD_ROWS];
};

#endif // MRE_H
