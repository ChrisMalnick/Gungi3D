/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Position.h
 * 
 * Summary:	Maintains a logical representation of the game board's layout for
 *		any given turn as well as a count of how many times it has occurred
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

#ifndef POSITION_H
#define POSITION_H

#include "Game/Board.h"

class Position
{
public:
	// Class functions
	// ---------------
	// Constructors
	Position() = default;
	Position(Board &p_board_ref);

	// Member functions
	// ----------------
	inline std::vector<int>& getStackRef(int x, int y) { return this->m_IDs[x][y]; }
	inline std::vector<int>& getStackRef(int p_index) { return this->m_IDs[this->getX(p_index)][this->getY(p_index)]; }

	inline int getID(int x, int y, int z) { return this->m_IDs[x][y][z]; }
	inline int getID(int p_index, int z) { return this->m_IDs[this->getX(p_index)][this->getY(p_index)][z]; }

	inline int getCount() { return this->m_count; }

	inline void setID(int p_ID, int x, int y) { this->m_IDs[x][y].push_back(p_ID); }
	inline void setID(int p_ID, int p_index) { this->m_IDs[this->getX(p_index)][this->getY(p_index)].push_back(p_ID); }

	inline void setCount(int p_count) { this->m_count = p_count; }

	inline void increment() { ++this->m_count; }

	bool equals(Position p_position);

private:
	inline int getX(int p_index) { return (p_index % BOARD_COLS); }
	inline int getY(int p_index) { return (p_index / BOARD_COLS); }

	// Member variables
	// ----------------
	std::vector<int> m_IDs[BOARD_ROWS][BOARD_COLS]; // Board configuration consisting of combination of piece faces and colors
	int m_count; // Number of times board configuration has occured
};

#endif // POSITION_H
