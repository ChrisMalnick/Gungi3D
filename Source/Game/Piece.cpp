/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Piece.cpp
 * 
 * Summary:	Maintains a logical representation of a game piece
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

#include "Game/Piece.h"

// Member functions
// ----------------
void game::Piece::build(int p_ID, Face p_front, Face p_back, Color p_color)
{
	this->m_ID = p_ID;

	this->m_front = p_front;
	this->m_back = p_back;

	this->m_color = p_color;
}

bool game::Piece::shallowEquals(Piece *p_piece_ptr)
{
	if (this->m_color != p_piece_ptr->getColor())
		return false;
	
	if (this->getSideUp() != p_piece_ptr->getSideUp())
		return false;

	return true;
}

bool game::Piece::equals(Piece *p_piece_ptr)
{
	if (this->m_front != p_piece_ptr->getFront())
		return false;

	if (this->m_back != p_piece_ptr->getBack())
		return false;
	
	if (this->m_color != p_piece_ptr->getColor())
		return false;

	if (this->m_side != p_piece_ptr->getSide())
		return false;

	return true;
}

int game::Piece::getWeight()
{
	Face face = this->getSideUp();

	static int pawn_offset = GOLD - BRONZE;
	static int lance_offset = FORTRESS - CATAPULT;

	if (face < PAWN)
		return WEIGHTS[face];

	if (face == PAWN)
		return WEIGHTS[face + this->getBack() - BRONZE];

	if (face > PAWN && face < LANCE)
		return WEIGHTS[face + pawn_offset];

	if (face == LANCE)
		return WEIGHTS[face + pawn_offset + this->getFront() - CATAPULT];

	if (face > LANCE)
		return WEIGHTS[face + pawn_offset + lance_offset];

	return 0;
}

std::vector<Move> game::Piece::getMoves(int x, int y, int z)
{
	std::vector<Move> moves;
	std::vector<Move> temp;

	int mod = (this->getAlignment() == WHITE ? -1 : 1);

	switch (this->getSideUp())
	{
	case BLANK:
		break;

	case COMMANDER:
		temp = this->getDiagonalMoves(x, y);
		moves.insert(moves.end(), temp.begin(), temp.end());

		temp = this->getOrthogonalMoves(x, y);
		moves.insert(moves.end(), temp.begin(), temp.end());

		break;

	case CAPTAIN:
		temp = this->getDiagonalMoves(x, y);
		moves.insert(moves.end(), temp.begin(), temp.end());

		if (z == 0 || z == 1)
			this->addMove(moves, x, y - 1 * mod);

		if (z == 1)
			this->addMove(moves, x, y + 1 * mod);

		if (z == 2)
		{
			this->addMove(moves, x - 2, y - 2 * mod);
			this->addMove(moves, x + 2, y - 2 * mod);
			this->addMove(moves, x - 2, y);
			this->addMove(moves, x + 2, y);
		}

		break;

	case SAMURAI:
		this->addMove(moves, x - 1, y - 1 * mod);
		this->addMove(moves, x + 1, y - 1 * mod);
		this->addMove(moves, x - 1, y);
		this->addMove(moves, x + 1, y);

		if (z == 0)
			this->addMove(moves, x, y - 1 * mod);

		else
		{
			this->addMove(moves, x, y - 2);
			this->addMove(moves, x, y + 2);
		}

		break;

	case SPY:
		this->addMove(moves, x - 1, y - 2 * mod);
		this->addMove(moves, x + 1, y - 2 * mod);

		if (z == 1 || z == 2)
		{
			this->addMove(moves, x - 1, y - 1 * mod);
			this->addMove(moves, x + 1, y - 1 * mod);
		}

		break;

	case CATAPULT:
		break;

	case FORTRESS:
		break;

	case HIDDEN_DRAGON:
		if (z == 0)
		{
			temp = this->getExtendedOrthogonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		else
		{
			temp = this->getDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case PRODIGY:
		if (z == 0)
		{
			temp = this->getExtendedDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		else
		{
			temp = this->getOrthogonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case BOW:
		if (z == 0 || z == 2)
		{
			this->addMove(moves, x - 2, y);
			this->addMove(moves, x + 2, y);
		}

		if (z == 1 || z == 2)
		{
			this->addMove(moves, x - 2, y - 2 * mod);
			this->addMove(moves, x + 2, y - 2 * mod);
		}

		if (z == 0)
			this->addMove(moves, x, y - 2 * mod);

		if (z == 1)
		{
			this->addMove(moves, x, y - 1);
			this->addMove(moves, x, y + 1);
		}

		if (z == 2)
			this->addMove(moves, x, y + 2 * mod);

		break;

	case PAWN:
		if (z == 0 || z == 1)
			this->addMove(moves, x, y - 1 * mod);

		if (z == 1 || z == 2)
		{
			this->addMove(moves, x - 2, y);
			this->addMove(moves, x + 2, y);
		}

		if (z == 2)
		{
			this->addMove(moves, x - 1, y - 1 * mod);
			this->addMove(moves, x + 1, y - 1 * mod);
		}

		break;

	case PISTOL:
		if (z == 0)
		{
			temp = this->getDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		else
		{
			temp = this->getOrthogonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case PIKE:
		if (z == 0)
		{
			temp = this->getOrthogonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());

			this->addMove(moves, x, y - 2 * mod);
		}

		else
		{
			temp = this->getDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case CLANDESTINITE:
		this->addMove(moves, x - 1, y - 2 * mod);
		this->addMove(moves, x + 1, y - 2 * mod);
		this->addMove(moves, x, y + 1 * mod);

		if (z == 1 || z == 2)
		{
			this->addMove(moves, x - 1, y - 1 * mod);
			this->addMove(moves, x + 1, y - 1 * mod);
		}

		if (z == 2)
		{
			this->addMove(moves, x - 2, y + 2 * mod);
			this->addMove(moves, x - 1, y + 2 * mod);
			this->addMove(moves, x + 1, y + 2 * mod);
			this->addMove(moves, x + 2, y + 2 * mod);
		}

		break;

	case LANCE:
		if (z == 0)
		{
			if (this->getAlignment() == BLACK)
			{
				for (int i = y - 1; i >= 0; --i)
					moves.push_back({ x, i });
			}

			else if (this->getAlignment() == WHITE)
			{
				for (int i = y + 1; i <= 8; ++i)
					moves.push_back({ x, i });
			}
		}

		else
		{
			temp = this->getDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case DRAGON_KING:
		temp = this->getDiagonalMoves(x, y);
		moves.insert(moves.end(), temp.begin(), temp.end());

		if (z == 0)
		{
			temp = this->getExtendedOrthogonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case PHOENIX:
		temp = this->getOrthogonalMoves(x, y);
		moves.insert(moves.end(), temp.begin(), temp.end());

		if (z == 0)
		{
			temp = this->getExtendedDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case ARROW:
		this->addMove(moves, x, y - 1);
		this->addMove(moves, x, y + 1);

		if (z == 0 || z == 2)
		{
			this->addMove(moves, x - 1, y + 1 * mod);
			this->addMove(moves, x + 1, y + 1 * mod);
		}

		if (z == 1 || z == 2)
		{
			this->addMove(moves, x - 2, y + 2 * mod);
			this->addMove(moves, x + 2, y + 2 * mod);
		}

		break;

	case BRONZE:
		this->addMove(moves, x - 1, y);
		this->addMove(moves, x + 1, y);

		break;

	case SILVER:
		if (z == 0)
		{
			temp = this->getOrthogonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		else
		{
			temp = this->getDiagonalMoves(x, y);
			moves.insert(moves.end(), temp.begin(), temp.end());
		}

		break;

	case GOLD:
		temp = this->getGoldMoves(x, y);
		moves.insert(moves.end(), temp.begin(), temp.end());

		break;
	}

	return moves;
}

std::vector<Move> game::Piece::getGoldMoves(int x, int y)
{
	std::vector<Move> moves;
	std::vector<Move> temp = this->getOrthogonalMoves(x, y);

	int mod = (this->getAlignment() == WHITE ? -1 : 1);

	moves.insert(moves.end(), temp.begin(), temp.end());

	this->addMove(moves, x - 1, y - 1 * mod);
	this->addMove(moves, x + 1, y - 1 * mod);

	return moves;
}

std::vector<Move> game::Piece::getDiagonalMoves(int x, int y)
{
	std::vector<Move> moves;

	this->addMove(moves, x - 1, y - 1);
	this->addMove(moves, x + 1, y - 1);
	this->addMove(moves, x - 1, y + 1);
	this->addMove(moves, x + 1, y + 1);

	return moves;
}

std::vector<Move> game::Piece::getOrthogonalMoves(int x, int y)
{
	std::vector<Move> moves;

	this->addMove(moves, x, y - 1);
	this->addMove(moves, x - 1, y);
	this->addMove(moves, x + 1, y);
	this->addMove(moves, x, y + 1);

	return moves;
}

std::vector<Move> game::Piece::getExtendedDiagonalMoves(int x, int y)
{
	std::vector<Move> moves;

	int i = x - 1;
	int j = y - 1;

	while (i >= LOWER_BOUND && j >= LOWER_BOUND)
		moves.push_back({ i--, j-- });

	i = x + 1;
	j = y - 1;

	while (i <= UPPER_BOUND && j >= LOWER_BOUND)
		moves.push_back({ i++, j-- });

	i = x - 1;
	j = y + 1;

	while (i >= LOWER_BOUND && j <= UPPER_BOUND)
		moves.push_back({ i--, j++ });

	i = x + 1;
	j = y + 1;

	while (i <= UPPER_BOUND && j <= UPPER_BOUND)
		moves.push_back({ i++, j++ });

	return moves;
}

std::vector<Move> game::Piece::getExtendedOrthogonalMoves(int x, int y)
{
	std::vector<Move> moves;

	for (int i = y - 1; i >= LOWER_BOUND; --i)
		moves.push_back({ x, i });

	for (int i = x - 1; i >= LOWER_BOUND; --i)
		moves.push_back({ i, y });

	for (int i = x + 1; i <= UPPER_BOUND; ++i)
		moves.push_back({ i, y });

	for (int i = y + 1; i <= UPPER_BOUND; ++i)
		moves.push_back({ x, i });

	return moves;
}
