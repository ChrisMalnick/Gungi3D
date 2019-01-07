/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Hand.cpp
 * 
 * Summary:	Maintains a logical representation of a player's hand
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

#include "Game/Hand.h"

// Class functions
// ---------------
// Constructor
Hand::Hand(game::Piece::Color p_color, game::Square::Location p_location, int *p_turn_ptr, game::Piece **p_piece_pp, game::Square **p_square_pp)
{
	this->m_color = p_color;
	this->m_location = p_location;

	this->m_turn_ptr = p_turn_ptr;

	this->m_curr_piece_pp = p_piece_pp;
	this->m_curr_square_pp = p_square_pp;
}

// Member functions
// ----------------
// Sets pieces in their initial positions
void Hand::init(Set &p_set_ref)
{
	this->m_piece_ptrs[0][0].push_back(p_set_ref.getPiecePtr(0 + this->m_color));

	this->m_piece_ptrs[1][0].push_back(p_set_ref.getPiecePtr(1 + this->m_color));
	this->m_piece_ptrs[1][0].push_back(p_set_ref.getPiecePtr(2 + this->m_color));

	this->m_piece_ptrs[2][0].push_back(p_set_ref.getPiecePtr(3 + this->m_color));
	this->m_piece_ptrs[2][0].push_back(p_set_ref.getPiecePtr(4 + this->m_color));

	this->m_piece_ptrs[3][0].push_back(p_set_ref.getPiecePtr(5 + this->m_color));
	this->m_piece_ptrs[3][0].push_back(p_set_ref.getPiecePtr(6 + this->m_color));
	this->m_piece_ptrs[3][0].push_back(p_set_ref.getPiecePtr(7 + this->m_color));

	this->m_piece_ptrs[0][1].push_back(p_set_ref.getPiecePtr(8 + this->m_color));

	this->m_piece_ptrs[1][1].push_back(p_set_ref.getPiecePtr(9 + this->m_color));

	this->m_piece_ptrs[2][1].push_back(p_set_ref.getPiecePtr(10 + this->m_color));

	this->m_piece_ptrs[3][1].push_back(p_set_ref.getPiecePtr(11 + this->m_color));

	this->m_piece_ptrs[0][2].push_back(p_set_ref.getPiecePtr(12 + this->m_color));
	this->m_piece_ptrs[0][2].push_back(p_set_ref.getPiecePtr(13 + this->m_color));

	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(14 + this->m_color));
	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(15 + this->m_color));
	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(16 + this->m_color));
	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(17 + this->m_color));
	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(18 + this->m_color));
	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(19 + this->m_color));
	this->m_piece_ptrs[1][2].push_back(p_set_ref.getPiecePtr(20 + this->m_color));

	this->m_piece_ptrs[2][2].push_back(p_set_ref.getPiecePtr(21 + this->m_color));

	this->m_piece_ptrs[3][2].push_back(p_set_ref.getPiecePtr(22 + this->m_color));
}

// Sets squares in their relative positions
void Hand::build(Set &p_set_ref)
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
			this->m_square_ptrs[j][i] = p_set_ref.getSquarePtr(j + i * HAND_COLS + this->m_location);
	}
}

std::vector<game::Piece*> Hand::getPiecePtrs()
{
	std::vector<game::Piece*> piece_ptrs;

	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (!this->m_piece_ptrs[j][i].empty())
				piece_ptrs.push_back(this->m_piece_ptrs[j][i].back());
		}
	}

	return piece_ptrs;
}

// For forced rearrangement
game::Piece* Hand::getMREPiecePtr()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (!this->m_piece_ptrs[j][i].empty() && this->m_piece_ptrs[j][i].back()->impartsMRE())
				return this->m_piece_ptrs[j][i].back();
		}
	}

	return nullptr;
}

// Returns pointer to square occupied by piece specified if found in hand
// Otherwise returns pointer to first empty square found in hand
game::Square* Hand::getSquarePtr(game::Piece *p_piece_ptr)
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (!this->m_piece_ptrs[j][i].empty() && this->m_piece_ptrs[j][i].back()->equals(p_piece_ptr))
				return this->m_square_ptrs[j][i];
		}
	}

	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (this->m_piece_ptrs[j][i].empty())
				return this->m_square_ptrs[j][i];
		}
	}

	return nullptr; // Should never reach this line
}

// Returns pointer to currently selected square
game::Square* Hand::getSelSquarePtr()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (this->m_square_ptrs[j][i]->getColor() == game::Square::GREEN)
				return this->m_square_ptrs[j][i];
		}
	}

	return nullptr;
}

// Removes one of specified piece
void Hand::remove(game::Piece *p_piece_ptr)
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (!this->m_piece_ptrs[j][i].empty() && this->m_piece_ptrs[j][i].back()->equals(p_piece_ptr))
			{
				this->m_piece_ptrs[j][i].pop_back();
				return;
			}
		}
	}
}

// Removes all pieces
void Hand::removeAll()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
			this->m_piece_ptrs[j][i].clear();
	}
}

// Clears all squares of specified color
void Hand::clear(game::Square::Color p_color)
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (this->m_square_ptrs[j][i]->getColor() == p_color)
				this->m_square_ptrs[j][i]->setColor(game::Square::CLEAR);
		}
	}
}

// Clears all squares
void Hand::clearAll()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
			this->m_square_ptrs[j][i]->setColor(game::Square::CLEAR);
	}
}

void Hand::setSwappable()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (this->m_square_ptrs[j][i]->getColor() == game::Square::CLEAR)
				this->m_square_ptrs[j][i]->setColor(game::Square::BLUE);
		}
	}
}

void Hand::mouseOver()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();
	
	// Check if this hand's turn
	if (this->accessible(*this->m_turn_ptr))
	{
		// No piece selected
		if (*this->m_curr_piece_pp == nullptr)
		{
			if (this->m_piece_ptrs[x][y].empty())
				(*this->m_curr_square_pp)->setColor(game::Square::GRAY);

			else
				(*this->m_curr_square_pp)->setColor(game::Square::GREEN);
		}

		// Board piece selected
		else if ((*this->m_curr_square_pp)->getColor() == game::Square::CLEAR)
			(*this->m_curr_square_pp)->setColor(game::Square::GRAY);
	}

	else
		(*this->m_curr_square_pp)->setColor(game::Square::GRAY);
}

void Hand::mouseButton1()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();

	// No piece selected
	if (*this->m_curr_piece_pp == nullptr)
	{
		// Select piece
		if ((*this->m_curr_square_pp)->getColor() == game::Square::GREEN)
		{
			*this->m_curr_piece_pp = this->m_piece_ptrs[x][y].back();
			this->setSwappable();
		}
	}

	else
	{
		// Swap stacks
		if ((*this->m_curr_square_pp)->getColor() == game::Square::BLUE)
		{
			this->m_piece_ptrs[x][y].swap(this->getStackRef(this->getSelSquarePtr()));
			*this->m_curr_piece_pp = nullptr;

			this->clearAll();
			(*this->m_curr_square_pp)->setColor(game::Square::GREEN);
		}

		// Deselect piece
		else if ((*this->m_curr_square_pp)->getColor() == game::Square::GREEN)
		{
			*this->m_curr_piece_pp = nullptr;
			this->clear(game::Square::BLUE);
		}
	}
}

bool Hand::empty()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (!this->m_piece_ptrs[j][i].empty())
				return false;
		}
	}

	return true;
}

bool Hand::constrained()
{
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			if (!this->m_piece_ptrs[j][i].empty() && this->m_piece_ptrs[j][i].back()->dropsFreely())
				return false;
		}
	}

	return true;
}
