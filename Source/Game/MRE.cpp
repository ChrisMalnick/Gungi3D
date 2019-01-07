/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	MRE.cpp
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

#include "Game/MRE.h"

// Member functions
// ----------------
void MRE::setRange(game::Piece *p_piece_ptr, int x, int y)
{
	if (!p_piece_ptr->impartsMRE())
		return;
	
	game::Piece::Face face = p_piece_ptr->getSideUp();
	game::Piece::Color color = p_piece_ptr->getColor();
	
	if (face == game::Piece::CATAPULT)
	{
		for (auto &elem : this->getCatapultRange(x, y))
		{
			if (elem.x >= LOWER_BOUND && elem.x <= UPPER_BOUND && elem.y >= this->getLowerBound(color) && elem.y <= this->getUpperBound(color))
				this->m_AOE[elem.x][elem.y].push_back({ game::Piece::CATAPULT, color });
		}
	}

	else if (face == game::Piece::FORTRESS)
	{
		if (color == game::Piece::BLACK)
		{
			for (int i = y; i >= LOWER_BOUND; --i)
				this->m_AOE[x][i].push_back({ game::Piece::FORTRESS, game::Piece::BLACK });
		}

		else if (color == game::Piece::WHITE)
		{
			for (int i = y; i <= UPPER_BOUND; ++i)
				this->m_AOE[x][i].push_back({ game::Piece::FORTRESS, game::Piece::WHITE });
		}
	}
}

void MRE::removeRange(game::Piece *p_piece_ptr)
{
	if (!p_piece_ptr->impartsMRE())
		return;

	game::Piece::Face face = p_piece_ptr->getSideUp();
	game::Piece::Color color = p_piece_ptr->getColor();
	
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			for (unsigned int k = 0; k < this->m_AOE[j][i].size(); ++k)
			{
				if (face != this->m_AOE[j][i][k].face || color != this->m_AOE[j][i][k].color)
					continue;

				this->m_AOE[j][i].erase(this->m_AOE[j][i].begin() + k);
				break;
			}
		}
	}
}

bool MRE::inRange(game::Piece::Color p_alignment, int x, int y)
{
	for (auto &elem : this->m_AOE[x][y])
	{
		if (p_alignment == elem.color)
			return true;
	}

	return false;
}

bool MRE::contains(game::Piece *p_piece_ptr, int x, int y)
{
	if (!p_piece_ptr->impartsMRE())
		return false;

	game::Piece::Face face = p_piece_ptr->getSideUp();
	game::Piece::Color color = p_piece_ptr->getColor();
	
	for (auto &elem : this->m_AOE[x][y])
	{
		if (face == elem.face && color == elem.color)
			return true;
	}

	return false;
}

std::vector<Coords2D> MRE::getCatapultRange(int x, int y)
{
	std::vector<Coords2D> range;

	range.push_back({ x, y - 2 });
	range.push_back({ x - 1, y - 1 });
	range.push_back({ x, y - 1 });
	range.push_back({ x + 1, y - 1 });
	range.push_back({ x - 2, y });
	range.push_back({ x - 1, y });
	range.push_back({ x, y });
	range.push_back({ x + 1, y });
	range.push_back({ x + 2, y });
	range.push_back({ x - 1, y + 1 });
	range.push_back({ x, y + 1 });
	range.push_back({ x + 1, y + 1 });
	range.push_back({ x, y + 2 });

	return range;
}
