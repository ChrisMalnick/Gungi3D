/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Position.cpp
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

#include "Game/Position.h"

// Class functions
// ---------------
// Constructor
Position::Position(Board &p_board_ref)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			for (auto &elem : p_board_ref.getStackRef(j, i))
				this->m_IDs[j][i].push_back(elem->getSideUp() + elem->getColor());
		}
	}

	this->m_count = 1;
}

// Member functions
// ----------------
bool Position::equals(Position p_position)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->m_IDs[j][i] != p_position.m_IDs[j][i])
				return false;
		}
	}

	return true;
}
