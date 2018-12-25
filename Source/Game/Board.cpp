/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Board.cpp
 *
 * Summary:	Maintains a logical representation of the game board and handles
 *			all game logic
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

#include "Game/Board.h"

// Class functions
// ---------------
// Constructor
Board::Board(int *p_turn_ptr, game::Piece **p_piece_pp, game::Square **p_square_pp, Hand *p_black_hand_ptr, Hand *p_white_hand_ptr)
{
	this->m_turn_ptr = p_turn_ptr;

	this->m_curr_piece_pp = p_piece_pp;
	this->m_curr_square_pp = p_square_pp;

	this->m_black_hand_ptr = p_black_hand_ptr;
	this->m_white_hand_ptr = p_white_hand_ptr;

	this->m_animate = true;
}

// Copy constructor
Board::Board(const Board &p_board_ref)
{
	std::copy(&p_board_ref.m_piece_ptrs[0][0], &p_board_ref.m_piece_ptrs[0][0] + BOARD_ROWS * BOARD_COLS, &this->m_piece_ptrs[0][0]);
	std::copy(&p_board_ref.m_square_ptrs[0][0], &p_board_ref.m_square_ptrs[0][0] + BOARD_ROWS * BOARD_COLS, &this->m_square_ptrs[0][0]);

	this->m_exchanges = p_board_ref.m_exchanges;

	this->m_MRE = p_board_ref.m_MRE;

	this->m_black_check = p_board_ref.m_black_check;
	this->m_white_check = p_board_ref.m_white_check;

	this->m_black_checkmate = p_board_ref.m_black_checkmate;
	this->m_white_checkmate = p_board_ref.m_white_checkmate;
}

// Member functions
// ----------------
void Board::init()
{
	this->removeAll();
	this->clearActions();

	this->m_selections.clear();
	this->m_exchanges.clear();
	this->m_animations.clear();

	this->m_black_check = false;
	this->m_white_check = false;

	this->m_black_checkmate = false;
	this->m_white_checkmate = false;
}

// Sets squares in their relative positions
void Board::build(Set &p_set_ref)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
			this->m_square_ptrs[j][i] = p_set_ref.getSquarePtr(j + i * BOARD_COLS + game::Square::BOARD);
	}
}

void Board::clear()
{
	this->m_selections.clear();

	this->clearActions();
	this->clearExchanges(*this->m_turn_ptr);
}

// Returns pointer to MRE imparting piece in tower at specified coordinates
game::Piece* Board::getMREPiecePtr(int x, int y)
{
	if (this->getHeight(x, y))
	{
		if (this->m_piece_ptrs[x][y][0]->impartsMRE())
			return this->m_piece_ptrs[x][y][0];
	}

	return nullptr;
}

// Returns pointer to currently selected square
game::Square* Board::getSelSquarePtr()
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->m_square_ptrs[j][i]->getColor() == game::Square::GREEN)
				return this->m_square_ptrs[j][i];
		}
	}

	return nullptr;
}

// Returns pointer to square containing commander of specified color
game::Square* Board::getCommSquarePtr(game::Piece::Color p_color)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			int height = this->getHeight(j, i);

			if (height == 0)
				continue;

			if (this->m_piece_ptrs[j][i][height - 1]->getSideUp() != game::Piece::COMMANDER)
				continue;

			if (this->m_piece_ptrs[j][i][height - 1]->getAlignment() != p_color)
				continue;

			return this->m_square_ptrs[j][i];
		}
	}

	return nullptr;
}

void Board::setPiecePtr(game::Piece *p_piece_ptr, int x, int y, int z)
{
	this->m_MRE.removeRange(this->m_piece_ptrs[x][y][z]);
	this->m_piece_ptrs[x][y][z] = p_piece_ptr;
	this->m_MRE.setRange(p_piece_ptr, x, y);
}

void Board::setPiecePtr(game::Piece *p_piece_ptr, game::Square *p_square_ptr, int z)
{
	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();
	
	this->m_MRE.removeRange(this->m_piece_ptrs[x][y][z]);
	this->m_piece_ptrs[x][y][z] = p_piece_ptr;
	this->m_MRE.setRange(p_piece_ptr, x, y);
}

// Sets specified piece on top of stack at specified coordinates
void Board::setPiecePtr(game::Piece *p_piece_ptr, int x, int y)
{
	this->m_piece_ptrs[x][y].push_back(p_piece_ptr);
	this->m_MRE.setRange(p_piece_ptr, x, y);
}

// Sets specified piece on top of stack at specified square
void Board::setPiecePtr(game::Piece *p_piece_ptr, game::Square *p_square_ptr)
{
	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	this->m_piece_ptrs[x][y].push_back(p_piece_ptr);
	this->m_MRE.setRange(p_piece_ptr, x, y);
}

// Inserts specified piece in stack at specified coordinates at specified index
void Board::insertPiecePtr(game::Piece *p_piece_ptr, int x, int y, int z)
{
	this->m_piece_ptrs[x][y].insert(this->m_piece_ptrs[x][y].begin() + z, p_piece_ptr);
	this->m_MRE.setRange(p_piece_ptr, x, y);
}

// Inserts specified piece in stack at specified square at specified index
void Board::insertPiecePtr(game::Piece *p_piece_ptr, game::Square *p_square_ptr, int z)
{
	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();
	
	this->m_piece_ptrs[x][y].insert(this->m_piece_ptrs[x][y].begin() + z, p_piece_ptr);
	this->m_MRE.setRange(p_piece_ptr, x, y);
}

void Board::flipPiecePtr(int x, int y, int z)
{
	this->m_MRE.removeRange(this->m_piece_ptrs[x][y][z]);
	this->m_piece_ptrs[x][y][z]->flip();
	this->m_MRE.setRange(this->m_piece_ptrs[x][y][z], x, y);
}

void Board::flipPiecePtr(game::Square *p_square_ptr, int z)
{
	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	this->m_MRE.removeRange(this->m_piece_ptrs[x][y][z]);
	this->m_piece_ptrs[x][y][z]->flip();
	this->m_MRE.setRange(this->m_piece_ptrs[x][y][z], x, y);
}

void Board::removePiecePtr(int x, int y, int z)
{
	this->m_MRE.removeRange(this->m_piece_ptrs[x][y][z]);
	this->m_piece_ptrs[x][y].erase(this->m_piece_ptrs[x][y].begin() + z);
}

void Board::removePiecePtr(game::Square *p_square_ptr, int z)
{
	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	this->m_MRE.removeRange(this->m_piece_ptrs[x][y][z]);
	this->m_piece_ptrs[x][y].erase(this->m_piece_ptrs[x][y].begin() + z);
}

// Removes topmost piece at specified coordinates
void Board::removePiecePtr(int x, int y)
{
	this->m_MRE.removeRange(this->m_piece_ptrs[x][y].back());
	this->m_piece_ptrs[x][y].pop_back();
}

// Removes topmost piece at specified square
void Board::removePiecePtr(game::Square *p_square_ptr)
{
	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	this->m_MRE.removeRange(this->m_piece_ptrs[x][y].back());
	this->m_piece_ptrs[x][y].pop_back();
}

// Removes all pieces
void Board::removeAll()
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			this->m_piece_ptrs[j][i].clear();
			this->m_MRE.getRangeRef(j, i).clear();
		}
	}
}

// Clears all squares of specified color
void Board::clear(game::Square::Color p_color)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->m_square_ptrs[j][i]->getColor() == p_color)
				this->m_square_ptrs[j][i]->setColor(game::Square::CLEAR);
		}
	}
}

// Clears all squares
void Board::clearAll()
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
			this->m_square_ptrs[j][i]->setColor(game::Square::CLEAR);
	}
}

void Board::clearActions()
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
			this->m_actions[j][i] = Action();
	}
}

void Board::clearExchanges(int p_turn)
{
	for (auto i = this->m_exchanges.begin(); i != this->m_exchanges.end();)
	{
		if (this->m_exchanges[i - this->m_exchanges.begin()].turn <= p_turn - 2)
			i = this->m_exchanges.erase(i);

		else
			++i;
	}
}

// Returns set of all in bound moves of topmost piece at specified coordinates
std::vector<Move> Board::getMoves(int x, int y)
{
	std::vector<Move> moves;

	if (int height = this->getHeight(x, y))
	{
		if (height == 1 || this->m_piece_ptrs[x][y][height - 2]->getAlignment() == this->m_piece_ptrs[x][y][height - 1]->getAlignment())
		{
			bool mod = 0;

			if (height == 3)
				mod = 1;

			else if (!this->m_piece_ptrs[x][y][height - 1]->receivesMRE())
				mod = 1;

			else if (!this->m_MRE.inRange(this->m_piece_ptrs[x][y][height - 1]->getAlignment(), x, y))
				mod = 1;

			moves = this->m_piece_ptrs[x][y][height - 1]->getMoves(x, y, height - mod);
		}

		else
			moves = this->m_piece_ptrs[x][y][height - 1]->getGoldMoves(x, y);
	}

	return moves;
}

// Sets color of all squares current piece can place into
void Board::setPlaceable()
{
	game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);

	int lower_bound = this->getLowerBound(active);
	int upper_bound = this->getUpperBound(active);

	for (auto &elem : this->m_selections)
	{
		if (elem.piece_ptr->shallowEquals(*this->m_curr_piece_pp))
		{
			for (int i = lower_bound; i <= upper_bound; ++i)
			{
				for (int j = 0; j < BOARD_COLS; ++j)
				{
					if (elem.settable[j][i])
						this->m_square_ptrs[j][i]->setColor(game::Square::BLUE);
				}
			}

			return;
		}
	}

	Selection selection;
	selection.piece_ptr = *this->m_curr_piece_pp;

	for (int i = lower_bound; i <= upper_bound; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (selection.settable[j][i] = this->placeable(*this->m_curr_piece_pp, j, i))
				this->m_square_ptrs[j][i]->setColor(game::Square::BLUE);
		}
	}

	this->m_selections.push_back(selection);
}

// Sets color of all squares current piece can drop into
void Board::setDroppable()
{
	Hand *active_hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	for (auto &elem : this->m_selections)
	{
		if (elem.piece_ptr->shallowEquals(*this->m_curr_piece_pp))
		{
			for (int i = 0; i < BOARD_ROWS; ++i)
			{
				for (int j = 0; j < BOARD_COLS; ++j)
				{
					if (elem.settable[j][i])
						this->m_square_ptrs[j][i]->setColor(game::Square::BLUE);
				}
			}

			return;
		}
	}

	Selection selection;
	selection.piece_ptr = *this->m_curr_piece_pp;

	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (selection.settable[j][i] = this->droppable(*this->m_curr_piece_pp, j, i, *this->m_turn_ptr, active_hand_ptr, passive_hand_ptr))
				this->m_square_ptrs[j][i]->setColor(game::Square::BLUE);
		}
	}

	this->m_selections.push_back(selection);
}

// Sets color of all squares previously captured MRE piece can occupy
void Board::setRearrangeable()
{
	game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);

	Hand *active_hand_ptr = this->getHandPtr(active);
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	for (int i = this->getLowerBound(active); i <= this->getUpperBound(active); ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->droppable(*this->m_curr_piece_pp, j, i, *this->m_turn_ptr, active_hand_ptr, passive_hand_ptr))
				this->m_square_ptrs[j][i]->setColor(game::Square::BLUE);
		}
	}
}

void Board::mouseOver()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();

	Hand *active_hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	if (*this->m_turn_ptr <= INITIAL_ARRANGEMENT)
	{
		if (this->m_square_ptrs[x][y]->getColor() == game::Square::CLEAR)
			this->m_square_ptrs[x][y]->setColor(game::Square::GRAY);
	}

	else
	{
		if (*this->m_curr_piece_pp == nullptr)
		{
			if (this->m_actions[x][y].color != game::Square::CLEAR)
				this->m_square_ptrs[x][y]->setColor(this->m_actions[x][y].color);
			
			else if (this->strikeable(x, y, *this->m_turn_ptr, active_hand_ptr, passive_hand_ptr))
				this->m_square_ptrs[x][y]->setColor(this->m_actions[x][y].color = game::Square::RED);

			else if (this->exchangeable(x, y, *this->m_turn_ptr, active_hand_ptr, passive_hand_ptr) || this->substitutable(x, y, *this->m_turn_ptr))
				this->m_square_ptrs[x][y]->setColor(this->m_actions[x][y].color = game::Square::BLUE);

			else if (this->selectable(x, y, *this->m_turn_ptr))
				this->m_square_ptrs[x][y]->setColor(this->m_actions[x][y].color = game::Square::GREEN);

			else
				this->m_square_ptrs[x][y]->setColor(this->m_actions[x][y].color = game::Square::GRAY);
		}

		else if (this->m_square_ptrs[x][y]->getColor() == game::Square::CLEAR)
			this->m_square_ptrs[x][y]->setColor(game::Square::GRAY);
	}

	if (*this->m_curr_piece_pp == nullptr)
		this->setMRERange(x, y);
}

void Board::mouseButton1()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();

	Hand *active_hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	switch (this->m_square_ptrs[x][y]->getColor())
	{
	case game::Square::GREEN:
		this->MB1Green(x, y, active_hand_ptr, passive_hand_ptr);
		break;

	case game::Square::BLUE:
		this->MB1Blue(x, y, active_hand_ptr, passive_hand_ptr);
		break;

	case game::Square::RED:
		this->MB1Red(x, y, active_hand_ptr, passive_hand_ptr);
		break;
	}
}

void Board::mouseButton2()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();

	Hand *active_hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	switch (this->m_square_ptrs[x][y]->getColor())
	{
	case game::Square::BLUE:
		this->MB2Blue(x, y, active_hand_ptr);
		break;

	case game::Square::RED:
		this->MB2Red(x, y, active_hand_ptr, passive_hand_ptr);
		break;
	}
}

void Board::mouseButton4()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();

	Hand *active_hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	if (this->m_square_ptrs[x][y]->getColor() == game::Square::RED)
	{
		if (*this->m_curr_piece_pp == nullptr)
		{
			for (int i = 1; i < this->getHeight(x, y); ++i)
			{
				if (this->downwards(x, y, i, *this->m_turn_ptr, active_hand_ptr, passive_hand_ptr))
				{
					this->strikeDown(x, y, i, active_hand_ptr, passive_hand_ptr);
					this->endTurn();

					break;
				}
			}
		}
	}
}

void Board::mouseButton5()
{
	int x = (*this->m_curr_square_pp)->getX();
	int y = (*this->m_curr_square_pp)->getY();

	Hand *active_hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	Hand *passive_hand_ptr = this->getHandPtr(this->getPassiveColor(*this->m_turn_ptr));

	if (this->m_square_ptrs[x][y]->getColor() == game::Square::RED)
	{
		if (*this->m_curr_piece_pp == nullptr)
		{
			for (int i = 0; i < this->getHeight(x, y) - 1; ++i)
			{
				if (this->upwards(x, y, i, *this->m_turn_ptr, active_hand_ptr, passive_hand_ptr))
				{
					this->strikeUp(x, y, i, active_hand_ptr, passive_hand_ptr);
					this->endTurn();

					break;
				}
			}
		}
	}
}

void Board::set(int x1, int y1, int x2, int y2, Hand *p_hand_ptr)
{
	this->setPiecePtr(p_hand_ptr->getPiecePtr(x1, y1), x2, y2);
	this->addAnimation(p_hand_ptr->getSquarePtr(x1, y1), this->m_square_ptrs[x2][y2], p_hand_ptr->getHeight(x1, y1) - 1, this->getHeight(x2, y2) - 1);
	p_hand_ptr->remove(x1, y1);
}

void Board::move(int x1, int y1, int x2, int y2, Hand *p_hand_ptr)
{
	if (this->recoverable(this->getPiecePtr(x1, y1), x2, y2, false))
		this->removeLat(x1, y1, x2, y2, p_hand_ptr);

	else
		this->transferLat(x1, y1, x2, y2);

	this->recover(x1, y1);
}

void Board::strike(int x1, int y1, int x2, int y2, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	int z1 = this->getHeight(x1, y1) - 1;
	int z2 = this->getHeight(x2, y2) - 1;

	bool bronze = this->m_piece_ptrs[x1][y1][z1]->getSideUp() == game::Piece::BRONZE;
	bool fortress = this->contains(game::Piece::FORTRESS, x2, y2);

	game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);

	if (this->recoverable(this->m_piece_ptrs[x1][y1][z1], x2, y2, true))
	{
		if (!this->m_piece_ptrs[x2][y2][z2]->impartsMRE())
			this->remove(x2, y2, z2, false, p_passive_hand_ptr);

		this->removeLat(x1, y1, x2, y2, p_active_hand_ptr);
	}

	else
	{
		if (this->m_piece_ptrs[x2][y2][z2]->getSideUp() == game::Piece::LANCE && (bronze || (this->territoryFull(active) && (z1 > 0 || (active == game::Piece::WHITE ? (y1 > WHITE_TERRITORY) : (y1 < BLACK_TERRITORY))))))
			this->remove(x2, y2, z2, false, p_passive_hand_ptr);

		else
			this->remove(x2, y2, z2, true, p_active_hand_ptr);

		if (bronze)
			this->betrayal(x2, y2, z2 - 1);

		this->transferLat(x1, y1, x2, y2);
	}

	if (fortress && !this->contains(game::Piece::FORTRESS, x2, y2))
		this->recover(this->getPassiveColor(*this->m_turn_ptr), x2);

	this->recover(x1, y1);
}

void Board::strikeDown(int x, int y, int z, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	bool bronze = this->m_piece_ptrs[x][y][z]->getSideUp() == game::Piece::BRONZE;
	bool fortress = this->contains(game::Piece::FORTRESS, x, y);

	int height = this->getHeight(x, y);

	if (z == height - 1 && this->recoverable(x, y, z - 1))
	{
		this->removeVert(x, y, z, z - 1, p_active_hand_ptr);

		if (!this->m_piece_ptrs[x][y][z - 1]->impartsMRE())
			this->remove(x, y, z - 1, false, p_passive_hand_ptr);
	}

	else
	{
		if (z == height - 1 && bronze)
			this->betrayal(x, y, z - 2);

		for (int i = z; i < height; ++i)
			this->transferVert(x, y, i, i - 1);

		if (this->m_piece_ptrs[x][y][z - 1]->getSideUp() == game::Piece::LANCE && (bronze || this->territoryFull(this->getActiveColor(*this->m_turn_ptr))))
			this->remove(x, y, z - 1, false, p_passive_hand_ptr);

		else
			this->remove(x, y, z - 1, true, p_active_hand_ptr);
	}

	if (fortress && !this->contains(game::Piece::FORTRESS, x, y))
		this->recover(this->getPassiveColor(*this->m_turn_ptr), x);

	this->recover(x, y);
}

void Board::strikeUp(int x, int y, int z, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	bool bronze = this->m_piece_ptrs[x][y][z]->getSideUp() == game::Piece::BRONZE;
	bool fortress = this->contains(game::Piece::FORTRESS, x, y);

	int height = this->getHeight(x, y);

	if (z == height - 2 && this->recoverable(x, y, z + 1))
	{
		this->remove(x, y, z + 1, false, p_passive_hand_ptr);
		this->remove(x, y, z, false, p_active_hand_ptr);
	}

	else
	{
		if (z == height - 2 && bronze)
			this->betrayal(x, y, z - 1);

		for (int i = z + 2; i < height; ++i)
			this->transferVert(x, y, i, i - 1);

		if (this->m_piece_ptrs[x][y][z + 1]->getSideUp() == game::Piece::LANCE && (bronze || this->territoryFull(this->getActiveColor(*this->m_turn_ptr))))
			this->remove(x, y, z + 1, false, p_passive_hand_ptr);

		else
			this->remove(x, y, z + 1, true, p_active_hand_ptr);
	}

	if (fortress && !this->contains(game::Piece::FORTRESS, x, y))
		this->recover(this->getPassiveColor(*this->m_turn_ptr), x);

	this->recover(x, y);
}

void Board::exchange(int x, int y, Hand *p_hand_ptr)
{
	this->addAnimation(this->m_square_ptrs[x][y], this->m_square_ptrs[x][y], 2, 0);

	if (this->recoverable(this->m_piece_ptrs[x][y][0], x, y, true))
	{
		this->removeVert(x, y, 0, 2, p_hand_ptr);

		this->insertPiecePtr(this->m_piece_ptrs[x][y][2], x, y, 0);
		this->removePiecePtr(x, y, 2);

		this->recover(x, y);
	}

	else
	{
		game::Piece *piece_ptr = this->m_piece_ptrs[x][y][2];

		this->addAnimation(this->m_square_ptrs[x][y], this->m_square_ptrs[x][y], 0, 2);
		this->setPiecePtr(this->m_piece_ptrs[x][y][0], x, y, 2);
		this->setPiecePtr(piece_ptr, x, y, 0);
	}

	this->m_exchanges.push_back({ this->m_square_ptrs[x][y], *this->m_turn_ptr });
}

void Board::substitute(int x, int y)
{
	game::Piece *piece_ptr = this->m_piece_ptrs[x][y][0];

	if (game::Square *square_ptr = this->getCommSquarePtr(piece_ptr->getAlignment()))
	{
		int z = this->getHeight(square_ptr) - 1;

		this->addAnimation(square_ptr, this->m_square_ptrs[x][y], z, 0);
		this->addAnimation(this->m_square_ptrs[x][y], square_ptr, 0, z);

		this->setPiecePtr(this->getPiecePtr(square_ptr, z), x, y, 0);
		this->setPiecePtr(piece_ptr, square_ptr, z);
	}
}

void Board::setSim(game::Piece *p_piece_ptr, int x, int y, Hand *p_hand_ptr)
{
	this->setPiecePtr(p_piece_ptr, x, y);
	p_hand_ptr->remove(p_piece_ptr);
}

void Board::moveSim(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	this->setPiecePtr(this->getPiecePtr(x1, y1), x2, y2);
	this->removePiecePtr(x1, y1);

	this->recoverSim(x1, y1, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

	if (this->recoverable(x2, y2))
		this->handleRecoverySim(x2, y2, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
}

void Board::strikeSim(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	this->m_temp_pieces.push_back(*this->getPiecePtr(x2, y2));

	bool bronze = this->getPiecePtr(x1, y1)->getSideUp() == game::Piece::BRONZE;
	bool fortress = this->contains(game::Piece::FORTRESS, x2, y2);

	this->m_temp_pieces.back().flip();
	p_active_hand_ptr->add(&this->m_temp_pieces.back());

	this->removePiecePtr(x2, y2);
	this->setPiecePtr(this->getPiecePtr(x1, y1), x2, y2);
	this->removePiecePtr(x1, y1);

	this->recoverSim(x1, y1, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

	if (this->recoverable(x2, y2))
	{
		this->switchHandsSim(p_active_hand_ptr, p_passive_hand_ptr);
		this->handleRecoverySim(x2, y2, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
	}

	else if (this->m_temp_pieces.back().impartsMRE() && (bronze || !this->rearrangeable(&this->m_temp_pieces.back(), p_turn, p_active_hand_ptr, p_passive_hand_ptr)))
		this->switchHandsSim(p_active_hand_ptr, p_passive_hand_ptr);

	if (bronze)
		this->betrayalSim(x2, y2, p_turn);

	if (fortress && !this->contains(game::Piece::FORTRESS, x2, y2))
		this->recoverSim(this->getPassiveColor(p_turn), x2, p_passive_hand_ptr);
}

void Board::strikeDownSim(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	this->m_temp_pieces.push_back(*this->getPiecePtr(x, y, z - 1));

	bool bronze = this->getPiecePtr(x, y, z)->getSideUp() == game::Piece::BRONZE;
	bool fortress = this->contains(game::Piece::FORTRESS, x, y);

	this->m_temp_pieces.back().flip();
	p_active_hand_ptr->add(&this->m_temp_pieces.back());

	this->removePiecePtr(x, y, z - 1);

	if (z == this->getHeight(x, y) && this->recoverable(x, y))
	{
		this->switchHandsSim(p_active_hand_ptr, p_passive_hand_ptr);
		this->handleRecoverySim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
	}

	else if (this->m_temp_pieces.back().impartsMRE() && (bronze || !this->rearrangeable(&this->m_temp_pieces.back(), p_turn, p_active_hand_ptr, p_passive_hand_ptr)))
		this->switchHandsSim(p_active_hand_ptr, p_passive_hand_ptr);

	this->recoverSim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

	if (z == this->getHeight(x, y) && bronze)
		this->betrayalSim(x, y, p_turn);

	if (fortress && !this->contains(game::Piece::FORTRESS, x, y))
		this->recoverSim(this->getPassiveColor(p_turn), x, p_passive_hand_ptr);
}

void Board::strikeUpSim(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	this->m_temp_pieces.push_back(*this->getPiecePtr(x, y, z + 1));

	bool bronze = this->getPiecePtr(x, y, z)->getSideUp() == game::Piece::BRONZE;
	bool fortress = this->contains(game::Piece::FORTRESS, x, y);

	this->m_temp_pieces.back().flip();
	p_active_hand_ptr->add(&this->m_temp_pieces.back());

	this->removePiecePtr(x, y, z + 1);

	if (z == this->getHeight(x, y) - 1 && this->recoverable(x, y))
	{
		this->switchHandsSim(p_active_hand_ptr, p_passive_hand_ptr);
		this->handleRecoverySim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
	}

	else if (this->m_temp_pieces.back().impartsMRE() && (bronze || !this->rearrangeable(&this->m_temp_pieces.back(), p_turn, p_active_hand_ptr, p_passive_hand_ptr)))
		this->switchHandsSim(p_active_hand_ptr, p_passive_hand_ptr);

	this->recoverSim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

	if (z == this->getHeight(x, y) - 1 && bronze)
		this->betrayalSim(x, y, p_turn);

	if (fortress && !this->contains(game::Piece::FORTRESS, x, y))
		this->recoverSim(this->getPassiveColor(p_turn), x, p_passive_hand_ptr);
}

void Board::exchangeSim(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	game::Piece *piece_ptr = this->m_piece_ptrs[x][y][0];

	this->setPiecePtr(this->getPiecePtr(x, y, 2), x, y, 0);
	this->setPiecePtr(piece_ptr, x, y, 2);

	this->recoverSim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

	this->m_exchanges.push_back({ this->m_square_ptrs[x][y], p_turn });
}

void Board::substituteSim(int x, int y)
{
	game::Piece *piece_ptr = this->m_piece_ptrs[x][y][0];

	if (game::Square *square_ptr = this->getCommSquarePtr(piece_ptr->getAlignment()))
	{
		int z = this->getHeight(square_ptr) - 1;

		this->setPiecePtr(this->getPiecePtr(square_ptr, z), x, y, 0);
		this->setPiecePtr(piece_ptr, square_ptr, z);
	}
}

void Board::betrayalSim(int x, int y, int p_turn)
{
	int z = this->getHeight(x, y) - 1;

	game::Piece::Color alignment = this->m_piece_ptrs[x][y][z]->getAlignment();

	for (int i = 0; i < z; ++i)
	{
		if (this->m_piece_ptrs[x][y][i]->getAlignment() == alignment)
			continue;

		// Betrayal does not affect pawns, bronze, silver, or gold
		if (this->m_piece_ptrs[x][y][i]->getFront() == game::Piece::PAWN)
			continue;

		// Betrayal only affects lances on first tier within attacker's territory
		if (this->m_piece_ptrs[x][y][i]->getSideUp() == game::Piece::LANCE && (i > 0 || (alignment == game::Piece::WHITE ? (y > WHITE_TERRITORY) : (y < BLACK_TERRITORY))))
			continue;

		this->m_temp_pieces.push_back(*this->m_piece_ptrs[x][y][i]);
		this->m_temp_pieces.back().flip();

		if (!this->contains(&this->m_temp_pieces.back(), x, y))
			this->setPiecePtr(&this->m_temp_pieces.back(), x, y, i);
	}
}

void Board::recoverSim(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (this->getHeight(x, y) == 0)
		return;

	if (!this->recoverable(x, y))
		return;

	game::Piece *piece_ptr = this->getPiecePtr(x, y);
	Hand *hand_ptr = (piece_ptr->getAlignment() == this->getActiveColor(p_turn) ? p_active_hand_ptr : p_passive_hand_ptr);

	hand_ptr->add(piece_ptr);
	this->removePiecePtr(x, y);

	this->recoverSim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
}

// Note: recovery in result of removing MRE logically only affects pieces in single tiered towers
void Board::recoverSim(game::Piece::Color p_MRE_color, int x, Hand *p_hand_ptr)
{
	int y = (p_MRE_color == game::Piece::WHITE ? (BLACK_TERRITORY + 1) : LOWER_BOUND);

	for (int i = y; i < y + 2; ++i)
	{
		if (!this->recoverable(p_MRE_color, x, i))
			continue;

		p_hand_ptr->add(this->getPiecePtr(x, i));
		this->removePiecePtr(x, i);
	}
}

void Board::switchHandsSim(Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	p_active_hand_ptr->remove(&this->m_temp_pieces.back());
	this->m_temp_pieces.back().flip();
	p_passive_hand_ptr->add(&this->m_temp_pieces.back());
}

void Board::handleRecoverySim(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	p_active_hand_ptr->add(this->getPiecePtr(x, y));
	this->removePiecePtr(x, y);
	this->recoverSim(x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
}

bool Board::rearrangeableLat(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (this->getPiecePtr(x2, y2)->getSideUp() != game::Piece::LANCE)
		return false;

	if (this->getPiecePtr(x1, y1)->getSideUp() == game::Piece::BRONZE)
		return false;

	if (this->recoverable(this->getPiecePtr(x1, y1), x2, y2, true))
		return false;

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	temp_board.strikeSim(x1, y1, x2, y2, p_turn, &temp_active_hand, &temp_passive_hand);

	if (!temp_active_hand.getMREPiecePtr())
		return false;

	return true;
}

bool Board::rearrangeableVert(int x, int y, int z1, int z2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (this->m_piece_ptrs[x][y][z2]->getSideUp() != game::Piece::LANCE)
		return false;

	if (this->m_piece_ptrs[x][y][z1]->getSideUp() == game::Piece::BRONZE)
		return false;

	int height = this->getHeight(x, y);

	if (z1 > z2 && z1 == height - 1 && this->recoverable(x, y, z2))
		return false;

	if (z1 < z2 && z1 == height - 2 && this->recoverable(x, y, z2))
		return false;

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	if (z1 > z2)
		temp_board.strikeDownSim(x, y, z1, p_turn, &temp_active_hand, &temp_passive_hand);

	if (z2 > z1)
		temp_board.strikeUpSim(x, y, z1, p_turn, &temp_active_hand, &temp_passive_hand);

	if (!temp_active_hand.getMREPiecePtr())
		return false;

	return true;
}

// Determines if topmost piece at specified coordinates is accessible on specified turn
bool Board::selectable(int x, int y, int p_turn)
{
	int height = this->getHeight(x, y);

	if (height == 0)
		return false;

	if (!this->m_piece_ptrs[x][y][height - 1]->accessible(p_turn))
		return false;

	return true;
}

// Determines if placing specified piece at specified coordinates is valid
// Note: initial arrangement must end with each player having exactly one pawn in every file
bool Board::placeable(game::Piece *p_piece_ptr, int x, int y)
{
	if (!this->stackable(p_piece_ptr, x, y))
		return false;

	game::Piece::Color alignment = p_piece_ptr->getAlignment();

	// Cannot place multiple pawns in any given file
	if (this->containsPawn(alignment, x))
	{
		if (p_piece_ptr->getSideUp() == game::Piece::PAWN)
			return false;
	}

	// Cannot fully occupy any given file without first placing pawn
	else if (p_piece_ptr->getSideUp() != game::Piece::PAWN)
	{
		if (this->openings(alignment, x) == 1)
			return false;

		if (p_piece_ptr->getSideUp() == game::Piece::COMMANDER && this->fullTowers(alignment, x) == 2)
			return false;
	}

	return true;
}

// Determines if dropping specified piece at specified coordinates is valid
bool Board::droppable(game::Piece *p_piece_ptr, int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	// Piece cannot drop into tower it cannot stack in
	if (!this->stackable(p_piece_ptr, x, y))
		return false;
	
	// When dropping into an occupied space
	if (int height = this->getHeight(x, y))
	{
		// Only pieces of same alignment can be dropped on
		if (this->m_piece_ptrs[x][y][height - 1]->getAlignment() != p_piece_ptr->getAlignment())
			return false;

		// Only pieces with earth-link ability can be dropped on
		if (!this->m_piece_ptrs[x][y][height - 1]->links())
			return false;

		// Only front pieces can be dropped on clandestinites and only back pieces can be dropped on spies
		if (this->m_piece_ptrs[x][y][height - 1]->getSideUp() == (p_piece_ptr->getSide() == game::Piece::BACK ? game::Piece::CLANDESTINITE : game::Piece::SPY))
			return false;
	}

	// Cannot drop into forced recovery
	if (this->recoverable(p_piece_ptr, x, y, false))
		return false;

	// Cannot drop if would leave in check
	if (this->dropLeavesInCheck(p_piece_ptr, x, y))
		return false;

	if (p_piece_ptr->getSideUp() == game::Piece::PAWN || p_piece_ptr->getSideUp() == game::Piece::BRONZE)
	{
		// Pawn and bronze cannot drop if already contained in file
		if (this->contains(p_piece_ptr, x))
			return false;

		// Pawn and bronze cannot drop if checkmate would be attained
		if (this->dropCheckmates(p_piece_ptr, x, y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
			return false;
	}

	return true;
}

// Determines if move between specified coordinates is possible
bool Board::moveable(int x1, int y1, int x2, int y2)
{
	game::Piece *piece_ptr = this->getPiecePtr(x1, y1);

	// Tower cannot contain multiple of any given piece
	if (this->contains(piece_ptr, x2, y2))
		return false;

	// File cannot contain multiple bronze
	if (x2 != x1 && piece_ptr->getSideUp() == game::Piece::BRONZE && this->contains(piece_ptr, x2))
		return false;

	// Cannot reach destination if blocked by other pieces
	if (this->blocked(x1, y1, x2, y2))
		return false;

	return true;
}

// Determines if moving between specified coordinates is valid
bool Board::moveable(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	int height = this->getHeight(x2, y2);

	// Cannot move into full tower
	if (height == MAX_HEIGHT)
		return false;

	// Cannot move on top of commander
	if (height && this->m_piece_ptrs[x2][y2][height - 1]->getSideUp() == game::Piece::COMMANDER)
		return false;

	// Commander cannot escape to occupied square when in check
	if (height && this->getPiecePtr(x1, y1)->getSideUp() == game::Piece::COMMANDER && this->check(this->getActiveColor(p_turn)))
		return false;

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	bool bronze = temp_board.getPiecePtr(x1, y1)->getSideUp() == game::Piece::BRONZE;

	temp_board.moveSim(x1, y1, x2, y2, p_turn, &temp_active_hand, &temp_passive_hand);

	// Bronze cannot move if checkmate would be attained
	if (bronze && temp_board.checkmate(p_turn + 1, &temp_passive_hand, &temp_active_hand))
		return false;

	// Cannot move if would leave in check
	if (temp_board.check(temp_board.getActiveColor(p_turn)))
		return false;

	return true;
}

// Determines if striking between specified coordinates is valid
bool Board::strikeable(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	int height = this->getHeight(x2, y2);

	// Cannot strike empty space
	if (height == 0)
		return false;

	// Cannot strike allied piece
	if (this->m_piece_ptrs[x2][y2][height - 1]->getAlignment() == this->getPiecePtr(x1, y1)->getAlignment())
		return false;

	// Commander cannot escape to occupied square when in check
	if (height > 1 && this->getPiecePtr(x1, y1)->getSideUp() == game::Piece::COMMANDER && this->check(this->getActiveColor(p_turn)))
		return false;

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	bool bronze = temp_board.getPiecePtr(x1, y1)->getSideUp() == game::Piece::BRONZE;

	temp_board.strikeSim(x1, y1, x2, y2, p_turn, &temp_active_hand, &temp_passive_hand);

	// Cannot strike if would leave in check
	if (temp_board.check(temp_board.getActiveColor(p_turn)) && !temp_active_hand.getMREPiecePtr())
		return false;

	// Bronze cannot strike if checkmate would be attained
	if (bronze && temp_board.checkmate(p_turn + 1, &temp_passive_hand, &temp_active_hand))
		return false;

	return true;
}

// Determines if 1-3 tier exchange is possible at specified coordinates on specified turn
bool Board::exchangeable(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	// Exchange can only occur in tiple tiered tower
	if (this->getHeight(x, y) != MAX_HEIGHT)
		return false;

	// Exchange can only occur if both top and bottom pieces are controlled
	if (!this->m_piece_ptrs[x][y][0]->accessible(p_turn) || !this->m_piece_ptrs[x][y][2]->accessible(p_turn))
		return false;

	// Exchange can only occur if captain is present at top or bottom of tower
	if (this->m_piece_ptrs[x][y][0]->getSideUp() != game::Piece::CAPTAIN && this->m_piece_ptrs[x][y][2]->getSideUp() != game::Piece::CAPTAIN)
		return false;

	// Exchange cannot occur if bottom piece is catapult or fortress or if top piece is commander
	if (this->m_piece_ptrs[x][y][0]->impartsMRE() || this->m_piece_ptrs[x][y][2]->getSideUp() == game::Piece::COMMANDER)
		return false;

	// Exchange cannot occur if in check
	if (this->check(this->getActiveColor(p_turn)))
		return false;

	for (auto &elem : this->m_exchanges)
	{
		// Exchange cannot occur multiple times consecutively at same square
		if (elem.square_ptr == this->m_square_ptrs[x][y])
			return false;
	}

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	temp_board.exchangeSim(x, y, p_turn, &temp_active_hand, &temp_passive_hand);

	// Cannot exchange if would leave in check
	if (temp_board.check(temp_board.getActiveColor(p_turn)))
		return false;

	return true;
}

// Determines if substitution is possible at specified coordinates on specified turn
bool Board::substitutable(int x, int y, int p_turn)
{
	// Substitution can only occur if source is single tiered tower
	if (this->getHeight(x, y) != 1)
		return false;

	// Substitution can only occur if source piece is controlled
	if (!this->m_piece_ptrs[x][y][0]->accessible(p_turn))
		return false;

	// Substitution can only occur if source piece is samurai
	if (this->m_piece_ptrs[x][y][0]->getSideUp() != game::Piece::SAMURAI)
		return false;

	// Substitution can only occur if in check
	if (!this->check(this->getActiveColor(p_turn)))
		return false;

	if (game::Square *comm_square_ptr = this->getCommSquarePtr(this->getActiveColor(p_turn)))
	{
		int x2 = comm_square_ptr->getX();
		int y2 = comm_square_ptr->getY();

		// Substitution can only occur if allied commander is orthogonally adjacent
		if ((std::abs(x2 - x) == 1 && y2 - y == 0) || (x2 - x == 0 && std::abs(y2 - y) == 1))
		{
			Board temp_board = *this;
			temp_board.substituteSim(x, y);

			// Cannot substitute if would leave in check
			if (!temp_board.check(temp_board.getActiveColor(p_turn)))
				return true;
		}
	}

	return false;
}

// Determines if striking downwards at specified coordinates is valid
bool Board::downwards(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	// Cannot strike down if attacking piece is uncontrolled
	if (!this->m_piece_ptrs[x][y][z]->accessible(p_turn))
		return false;

	// Cannot strike down if target piece shares alignment with attacking piece
	if (this->m_piece_ptrs[x][y][z]->getAlignment() == this->m_piece_ptrs[x][y][z - 1]->getAlignment())
		return false;

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	bool bronze = temp_board.getPiecePtr(x, y, z)->getSideUp() == game::Piece::BRONZE;

	temp_board.strikeDownSim(x, y, z, p_turn, &temp_active_hand, &temp_passive_hand);

	// Cannot strike down if would leave in check
	if (temp_board.check(temp_board.getActiveColor(p_turn)) && !temp_active_hand.getMREPiecePtr())
		return false;

	// Bronze cannot strike down if checkmate would be attained
	if (bronze && temp_board.checkmate(p_turn + 1, &temp_passive_hand, &temp_active_hand))
		return false;

	return true;
}

// Determines if striking upwards at specified coordinates is valid
bool Board::upwards(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	// Fortress cannot make immobile strike
	if (z == 0 && this->m_piece_ptrs[x][y][0]->getSideUp() == game::Piece::FORTRESS)
		return false;
	
	// Cannot strike up if attacking piece is uncontrolled
	if (!this->m_piece_ptrs[x][y][z]->accessible(p_turn))
		return false;

	// Cannot strike up if target piece shares alignment with attacking piece
	if (this->m_piece_ptrs[x][y][z]->getAlignment() == this->m_piece_ptrs[x][y][z + 1]->getAlignment())
		return false;

	Board temp_board = *this;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	bool bronze = temp_board.getPiecePtr(x, y, z)->getSideUp() == game::Piece::BRONZE;

	temp_board.strikeUpSim(x, y, z, p_turn, &temp_active_hand, &temp_passive_hand);

	// Cannot strike up if would leave in check
	if (temp_board.check(temp_board.getActiveColor(p_turn)) && !temp_active_hand.getMREPiecePtr())
		return false;

	// Bronze cannot strike up if checkmate would be attained
	if (bronze && temp_board.checkmate(p_turn + 1, &temp_passive_hand, &temp_active_hand))
		return false;

	return true;
}

// Determines if specified color is in check
bool Board::check(game::Piece::Color p_color)
{
	game::Square *comm_square_ptr = this->getCommSquarePtr(p_color);

	if (!comm_square_ptr)
		return false;

	int x = comm_square_ptr->getX();
	int y = comm_square_ptr->getY();

	int z = this->getHeight(x, y) - 1;

	if (z > 0 && this->m_piece_ptrs[x][y][z - 1]->getAlignment() != p_color && this->m_piece_ptrs[x][y][z - 1]->getSideUp() != game::Piece::FORTRESS)
		return true;

	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			int height = this->getHeight(j, i);

			if (height == 0)
				continue;

			if (this->m_piece_ptrs[j][i][height - 1]->getAlignment() == p_color)
				continue;

			for (auto &elem : this->getMoves(j, i))
			{
				if (!this->moveable(j, i, elem.x, elem.y))
					continue;

				if (elem.x == x && elem.y == y)
					return true;
			}
		}
	}

	return false;
}

// Sets color of all squares in range of topmost piece at specified coordinates
void Board::setMoveable(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (this->m_actions[x][y].selected)
	{
		for (int i = 0; i < BOARD_ROWS; ++i)
		{
			for (int j = 0; j < BOARD_COLS; ++j)
				this->m_square_ptrs[j][i]->setColor(this->m_actions[x][y].moves[j][i]);
		}
	}

	else
	{
		for (auto &elem : this->getMoves(x, y))
		{
			if (!this->moveable(x, y, elem.x, elem.y))
				continue;

			if (this->strikeable(x, y, elem.x, elem.y, *this->m_turn_ptr, p_active_hand_ptr, p_passive_hand_ptr))
				this->m_square_ptrs[elem.x][elem.y]->setColor(this->m_actions[x][y].moves[elem.x][elem.y] = game::Square::RED);

			else if (this->moveable(x, y, elem.x, elem.y, *this->m_turn_ptr, p_active_hand_ptr, p_passive_hand_ptr))
				this->m_square_ptrs[elem.x][elem.y]->setColor(this->m_actions[x][y].moves[elem.x][elem.y] = game::Square::BLUE);
		}

		this->m_actions[x][y].selected = true;
	}
}

// Sets color of all squares in range of MRE piece in tower at specified coordinates
void Board::setMRERange(int x, int y)
{
	if (game::Piece *piece_ptr = this->getMREPiecePtr(x, y))
	{
		for (int i = 0; i < BOARD_ROWS; ++i)
		{
			for (int j = 0; j < BOARD_COLS; ++j)
			{
				if (this->m_MRE.contains(piece_ptr, j, i))
				{
					if (this->m_square_ptrs[j][i]->getColor() == game::Square::CLEAR)
						this->m_square_ptrs[j][i]->setColor(game::Square::PURPLE);
				}
			}
		}
	}
}

void Board::MB1Green(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (*this->m_curr_piece_pp == nullptr)
		this->select(x, y, p_active_hand_ptr, p_passive_hand_ptr);

	else
		this->deselect();
}

void Board::MB1Blue(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (*this->m_curr_piece_pp == nullptr)
		this->select(x, y, p_active_hand_ptr, p_passive_hand_ptr);

	else
	{
		if (game::Square *square_ptr = this->getSelSquarePtr())
		{
			this->move(square_ptr->getX(), square_ptr->getY(), x, y, p_active_hand_ptr);
			this->endTurn();
		}

		else if (game::Square *square_ptr = p_active_hand_ptr->getSquarePtr(*this->m_curr_piece_pp))
		{
			this->set(square_ptr->getX(), square_ptr->getY(), x, y, p_active_hand_ptr);
			this->endTurn();
		}
	}
}

void Board::MB1Red(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (*this->m_curr_piece_pp == nullptr)
	{
		if (this->selectable(x, y, *this->m_turn_ptr))
			this->select(x, y, p_active_hand_ptr, p_passive_hand_ptr);
	}

	else if (game::Square *sel_square_ptr = this->getSelSquarePtr())
	{
		this->strike(sel_square_ptr->getX(), sel_square_ptr->getY(), x, y, p_active_hand_ptr, p_passive_hand_ptr);
		this->endTurn();
	}
}

void Board::MB2Blue(int x, int y, Hand *p_hand_ptr)
{
	if (*this->m_curr_piece_pp == nullptr)
	{
		switch (this->getHeight(x, y))
		{
		case MAX_HEIGHT:
			this->exchange(x, y, p_hand_ptr);
			this->endTurn();

			break;

		case 1:
			this->substitute(x, y);
			this->endTurn();

			break;
		}
	}
}

void Board::MB2Red(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	if (*this->m_curr_piece_pp != nullptr)
	{
		if (game::Square *square_ptr = this->getSelSquarePtr())
		{
			if (this->moveable(square_ptr->getX(), square_ptr->getY(), x, y, *this->m_turn_ptr, p_active_hand_ptr, p_passive_hand_ptr))
			{
				this->move(square_ptr->getX(), square_ptr->getY(), x, y, p_active_hand_ptr);
				this->endTurn();
			}
		}
	}

	else if (this->exchangeable(x, y, *this->m_turn_ptr, p_active_hand_ptr, p_passive_hand_ptr))
	{
		this->exchange(x, y, p_active_hand_ptr);
		this->endTurn();
	}
}

void Board::select(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	*this->m_curr_piece_pp = this->getPiecePtr(x, y);

	this->clearAll();
	this->setMoveable(x, y, p_active_hand_ptr, p_passive_hand_ptr);

	this->m_square_ptrs[x][y]->setColor(game::Square::GREEN);
}

void Board::deselect()
{
	*this->m_curr_piece_pp = nullptr;

	this->clearAll();
	this->mouseOver();
}

void Board::betrayal(int x, int y, int z)
{
	game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);

	for (int i = 0; i <= z; ++i)
	{
		if (this->m_piece_ptrs[x][y][i]->getAlignment() == active)
			continue;

		// Betrayal does not affect pawns, bronze, silver, or gold
		if (this->m_piece_ptrs[x][y][i]->getFront() == game::Piece::PAWN)
			continue;

		// Betrayal only affects lances on first tier within attacker's territory
		if (this->m_piece_ptrs[x][y][i]->getSideUp() == game::Piece::LANCE && (i > 0 || (active == game::Piece::WHITE ? (y > WHITE_TERRITORY) : (y < BLACK_TERRITORY))))
			continue;

		game::Piece temp_piece = *this->m_piece_ptrs[x][y][i];
		temp_piece.flip();

		if (!this->contains(&temp_piece, x, y))
		{
			this->flipPiecePtr(x, y, i);
			this->addAnimation(this->m_square_ptrs[x][y], i);
		}
	}
}

void Board::recover(int x, int y)
{
	int z = this->getHeight(x, y) - 1;

	if (z < 0)
		return;

	if (!this->recoverable(x, y))
		return;

	this->remove(x, y, z, false, this->getHandPtr(this->m_piece_ptrs[x][y][z]->getAlignment()));
	this->recover(x, y);
}

void Board::recover(game::Piece::Color p_color, int x)
{
	int y = (p_color == game::Piece::WHITE ? (BLACK_TERRITORY + 1) : LOWER_BOUND);

	for (int i = y; i < y + 2; ++i)
	{
		if (this->recoverable(p_color, x, i))
			this->remove(x, i, 0, false, this->getHandPtr(p_color));
	}
}

void Board::transferLat(int x1, int y1, int x2, int y2)
{
	this->setPiecePtr(this->getPiecePtr(x1, y1), x2, y2);
	this->addAnimation(this->m_square_ptrs[x1][y1], this->m_square_ptrs[x2][y2], this->getHeight(x1, y1) - 1, this->getHeight(x2, y2) - 1);
	this->removePiecePtr(x1, y1);
}

void Board::transferVert(int x, int y, int z1, int z2)
{
	game::Piece *piece_ptr = this->m_piece_ptrs[x][y][z2];
	
	this->setPiecePtr(this->m_piece_ptrs[x][y][z1], x, y, z2);
	this->addAnimation(this->m_square_ptrs[x][y], this->m_square_ptrs[x][y], z1, z2);
	this->setPiecePtr(piece_ptr, x, y, z2);
}

void Board::remove(int x, int y, int z, bool p_flip, Hand *p_hand_ptr)
{
	if (p_flip)
	{
		game::Piece temp_piece = *this->m_piece_ptrs[x][y][z];
		temp_piece.flip();

		game::Square *square_ptr = p_hand_ptr->getSquarePtr(&temp_piece);
		p_hand_ptr->add(&temp_piece, square_ptr);

		this->addAnimation(this->m_square_ptrs[x][y], square_ptr, z, p_hand_ptr->getHeight(square_ptr) - 1);
		p_hand_ptr->remove(square_ptr);

		this->flipPiecePtr(x, y, z);
		p_hand_ptr->add(this->m_piece_ptrs[x][y][z], square_ptr);
		this->removePiecePtr(x, y, z);
	}

	else
	{
		game::Square *square_ptr = p_hand_ptr->getSquarePtr(this->m_piece_ptrs[x][y][z]);
		p_hand_ptr->add(this->m_piece_ptrs[x][y][z], square_ptr);

		this->addAnimation(this->m_square_ptrs[x][y], square_ptr, nullptr, z, p_hand_ptr->getHeight(square_ptr) - 1, NULL);
		this->removePiecePtr(x, y, z);
	}
}

void Board::removeLat(int x1, int y1, int x2, int y2, Hand *p_hand_ptr)
{
	game::Square *square_ptr = p_hand_ptr->getSquarePtr(this->getPiecePtr(x1, y1));

	this->setPiecePtr(this->getPiecePtr(x1, y1), x2, y2);
	p_hand_ptr->add(this->getPiecePtr(x1, y1), square_ptr);

	this->addAnimation(this->m_square_ptrs[x1][y1], square_ptr, this->m_square_ptrs[x2][y2], this->getHeight(x1, y1) - 1, p_hand_ptr->getHeight(square_ptr) - 1, this->getHeight(x2, y2) - 1);

	this->removePiecePtr(x1, y1);
	this->removePiecePtr(x2, y2);
}

void Board::removeVert(int x, int y, int z1, int z2, Hand *p_hand_ptr)
{
	game::Piece *piece_ptr = this->m_piece_ptrs[x][y][z2];
	game::Square *square_ptr = p_hand_ptr->getSquarePtr(this->m_piece_ptrs[x][y][z1]);

	this->setPiecePtr(this->m_piece_ptrs[x][y][z1], x, y, z2);
	p_hand_ptr->add(this->m_piece_ptrs[x][y][z1], square_ptr);

	this->addAnimation(this->m_square_ptrs[x][y], square_ptr, this->m_square_ptrs[x][y], z1, p_hand_ptr->getHeight(square_ptr) - 1, z2);

	this->setPiecePtr(piece_ptr, x, y, z2);
	this->removePiecePtr(x, y, z1);
}

void Board::endTurn()
{
	if (*this->m_turn_ptr >= INITIAL_ARRANGEMENT)
	{
		this->m_black_check = this->check(game::Piece::BLACK);
		this->m_white_check = this->check(game::Piece::WHITE);
	}
	
	if ((*this->m_curr_square_pp)->getColor() == game::Square::RED)
	{
		if (*this->m_curr_piece_pp = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr))->getMREPiecePtr())
		{
			this->clearAll();
			this->setRearrangeable();
			this->mouseOver();

			return;
		}
	}

	this->clear();

	++(*this->m_turn_ptr);

	this->deselect();

	this->m_black_hand_ptr->clearAll();
	this->m_white_hand_ptr->clearAll();

	if (*this->m_turn_ptr > INITIAL_ARRANGEMENT)
	{
		game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);
		game::Piece::Color passive = this->getPassiveColor(*this->m_turn_ptr);
		
		if (!(this->getCheckmateRef(passive) = this->getCheckRef(passive)))
			this->getCheckmateRef(active) = this->checkmate(*this->m_turn_ptr, this->getHandPtr(active), this->getHandPtr(passive));
	}
}

// Determines if specified piece is stackable at specified coordinates
bool Board::stackable(game::Piece *p_piece_ptr, int x, int y)
{
	int height = this->getHeight(x, y);

	// Tower cannot exceed maximum height
	if (height == MAX_HEIGHT)
		return false;

	// Tower cannot contain multiple of any given piece
	if (this->contains(p_piece_ptr, x, y))
		return false;

	if (height)
	{
		// Cannot stack atop commander
		if (this->m_piece_ptrs[x][y][height - 1]->getSideUp() == game::Piece::COMMANDER)
			return false;

		// Cannot stack catapult or fortress atop other pieces
		if (p_piece_ptr->impartsMRE())
			return false;
	}

	return true;
}

// Determines if immobile strike is possible at specified coordinates on specified turn
bool Board::strikeable(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	int height = this->getHeight(x, y);

	// Strike can only occur in multiple tiered tower
	if (height < 2)
		return false;

	for (int i = 0; i < height; ++i)
	{
		if (i > 0 && this->downwards(x, y, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
			return true;

		if (i < height - 1 && this->upwards(x, y, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
			return true;
	}

	return false;
}

// Determines if specified piece has at least one droppable square in own territory
bool Board::rearrangeable(game::Piece *p_piece_ptr, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	game::Piece::Color active = this->getActiveColor(p_turn);

	for (int i = this->getLowerBound(active); i <= this->getUpperBound(active); ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->droppable(p_piece_ptr, j, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
				return true;
		}
	}

	return false;
}

bool Board::recoverable(game::Piece *p_piece_ptr, int x, int y, bool p_remove)
{
	Board temp_board = *this;

	if (p_remove)
		temp_board.removePiecePtr(x, y);

	temp_board.setPiecePtr(p_piece_ptr, x, y);

	if (temp_board.recoverable(x, y))
		return true;

	return false;
}

bool Board::recoverable(int x, int y, int z)
{
	Board temp_board = *this;
	temp_board.removePiecePtr(x, y, z);

	if (temp_board.recoverable(x, y))
		return true;

	return false;
}

// Determines if topmost piece at specified coordinates is immoveable
bool Board::recoverable(int x, int y)
{
	game::Piece *piece_ptr = this->getPiecePtr(x, y);

	if (!piece_ptr->recovers())
		return false;

	// Only pieces within last two rows relative to alignment can recover
	if (piece_ptr->getAlignment() == game::Piece::WHITE ? !(y == UPPER_BOUND || y == UPPER_BOUND - 1) : !(y == LOWER_BOUND || y == LOWER_BOUND + 1))
		return false;

	// Only pieces without available moves can recover
	// (Moves relative to piece's effective height regardless of arrangement)
	if (!this->getMoves(x, y).empty())
		return false;

	return true;
}

// Determines if topmost piece at specified coordinates is immoveable as a result of MRE of specified color being removed
bool Board::recoverable(game::Piece::Color p_color, int x, int y)
{
	// Removing MRE can only result in forced recovery in single tiered towers
	if (this->getHeight(x, y) != 1)
		return false;

	// Removing MRE can only result in forced recovery for pieces sharing alignment
	if (this->m_piece_ptrs[x][y][0]->getAlignment() != p_color)
		return false;

	if (!this->m_piece_ptrs[x][y][0]->recovers())
		return false;

	// Only pieces without available moves can recover
	// (Moves relative to piece's effective height regardless of arrangement)
	if (!this->getMoves(x, y).empty())
		return false;

	return true;
}

// Determines if active color on specified turn is checkmated
bool Board::checkmate(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, bool p_deferred)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			for (auto &elem : p_active_hand_ptr->getPiecePtrs())
			{
				if ((elem->getSideUp() == game::Piece::PAWN || elem->getSideUp() == game::Piece::BRONZE) != p_deferred)
					continue;

				if (this->droppable(elem, j, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
					return false;
			}

			if (this->selectable(j, i, p_turn) && ((this->getPiecePtr(j, i)->getSideUp() == game::Piece::BRONZE) == p_deferred))
			{
				for (auto &elem : this->getMoves(j, i))
				{
					if (!this->moveable(j, i, elem.x, elem.y))
						continue;

					if (this->moveable(j, i, elem.x, elem.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
						return false;

					if (this->strikeable(j, i, elem.x, elem.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
						return false;
				}
			}

			if (!p_deferred && this->exchangeable(j, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
				return false;

			if (!p_deferred && this->substitutable(j, i, p_turn))
				return false;

			int height = this->getHeight(j, i);

			if (height < 2)
				continue;

			for (int k = 0; k < height; ++k)
			{
				if ((this->m_piece_ptrs[j][i][k]->getSideUp() == game::Piece::BRONZE) != p_deferred)
					continue;

				if (k > 0 && this->downwards(j, i, k, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
					return false;

				if (k < height - 1 && this->upwards(j, i, k, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
					return false;
			}
		}
	}

	return true;
}

// Determines if dropping specified piece at specified coordinates leaves in check
bool Board::dropLeavesInCheck(game::Piece *p_piece_ptr, int x, int y)
{
	Board temp_board = *this;
	temp_board.setPiecePtr(p_piece_ptr, x, y);

	if (!temp_board.check(p_piece_ptr->getAlignment()))
		return false;

	return true;
}

// Determines if dropping specified piece at specified coordinates attains checkmate
bool Board::dropCheckmates(game::Piece *p_piece_ptr, int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr)
{
	Board temp_board = *this;
	Hand temp_active_hand = *p_active_hand_ptr;

	temp_board.setSim(p_piece_ptr, x, y, &temp_active_hand);

	if (temp_board.checkmate(p_turn + 1, p_passive_hand_ptr, &temp_active_hand))
		return true;

	return false;
}

// Move is blocked if piece lies along movement path
// Spys, bows, and clandestinites can jump over other pieces
// Enemy pieces within own MRE cannot be jumped over
bool Board::blocked(int x1, int y1, int x2, int y2)
{
	game::Piece *piece_ptr = this->getPiecePtr(x1, y1);
	game::Piece::Color alignment = piece_ptr->getAlignment();

	int xdelta = x2 - x1;
	int ydelta = y2 - y1;

	// For moves intermediate to orthogonal and diagonal
	// (One orthogonal followed by one diagonal in same direction)
	// Only spy and clandestinite have these moves
	// None of these moves are horizontally oriented (+/-2, +/-1)
	if (std::abs(xdelta) == 1 && std::abs(ydelta) == 2)
	{
		// Check if orthogonal square is blocked by MRE (spy and clandestinite can jump)
		if (this->MREBlocked(alignment, x1, y1 + (ydelta < 0 ? -1 : 1)))
			return true;
	}

	else if (xdelta < 0 && ydelta < 0)
	{
		for (int i = 1; x2 + i < x1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2 + i, y2 + i) : this->getHeight(x2 + i, y2 + i))
				return true;
		}
	}

	else if (xdelta == 0 && ydelta < 0)
	{
		for (int i = 1; y2 + i < y1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2, y2 + i) : this->getHeight(x2, y2 + i))
				return true;
		}
	}

	else if (xdelta > 0 && ydelta < 0)
	{
		for (int i = 1; x2 - i > x1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2 - i, y2 + i) : this->getHeight(x2 - i, y2 + i))
				return true;
		}
	}

	else if (xdelta < 0 && ydelta == 0)
	{
		for (int i = 1; x2 + i < x1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2 + i, y2) : this->getHeight(x2 + i, y2))
				return true;
		}
	}

	else if (xdelta > 0 && ydelta == 0)
	{
		for (int i = 1; x2 - i > x1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2 - i, y2) : this->getHeight(x2 - i, y2))
				return true;
		}
	}

	else if (xdelta < 0 && ydelta > 0)
	{
		for (int i = 1; x2 + i < x1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2 + i, y2 - i) : this->getHeight(x2 + i, y2 - i))
				return true;
		}
	}

	else if (xdelta == 0 && ydelta > 0)
	{
		for (int i = 1; y2 - i > y1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2, y2 - i) : this->getHeight(x2, y2 - i))
				return true;
		}
	}

	else if (xdelta > 0 && ydelta > 0)
	{
		for (int i = 1; x2 - i > x1; ++i)
		{
			if (piece_ptr->jumps() ? this->MREBlocked(alignment, x2 - i, y2 - i) : this->getHeight(x2 - i, y2 - i))
				return true;
		}
	}

	return false;
}

// Determines if topmost piece at specified coordinates is not aligned and is within own MRE
bool Board::MREBlocked(game::Piece::Color p_alignment, int x, int y)
{
	if (this->getHeight(x, y) == 0)
		return false;

	game::Piece::Color alignment = this->getPiecePtr(x, y)->getAlignment();

	if (alignment == p_alignment)
		return false;

	if (!this->m_MRE.inRange(alignment, x, y))
		return false;

	return true;
}

bool Board::territoryFull(game::Piece::Color p_color)
{
	for (int i = this->getLowerBound(p_color); i <= this->getUpperBound(p_color); ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->getHeight(j, i) == 0)
				return false;
		}
	}

	return true;
}

// Determines if tower at specified coordinates contains piece of specified face
bool Board::contains(game::Piece::Face p_face, int x, int y)
{
	for (auto &elem : this->m_piece_ptrs[x][y])
	{
		if (elem->getSideUp() == p_face)
			return true;
	}

	return false;
}

// Determines if tower at specified square contains piece of specified face
bool Board::contains(game::Piece::Face p_face, game::Square *p_square_ptr)
{
	for (auto &elem : this->getStackRef(p_square_ptr))
	{
		if (elem->getSideUp() == p_face)
			return true;
	}

	return false;
}

// Determines if tower at specified coordinates contains specified piece
bool Board::contains(game::Piece *p_piece_ptr, int x, int y)
{
	for (auto &elem : this->m_piece_ptrs[x][y])
	{
		if (elem->shallowEquals(p_piece_ptr))
			return true;
	}

	return false;
}

// Determines if specified file contains specified piece
bool Board::contains(game::Piece *p_piece_ptr, int x)
{
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (auto &elem : this->m_piece_ptrs[x][i])
		{
			if (elem->shallowEquals(p_piece_ptr))
				return true;
		}
	}

	return false;
}

// Determines if specified file contains pawn within specified color's territory
bool Board::containsPawn(game::Piece::Color p_color, int x)
{
	for (int i = this->getLowerBound(p_color); i <= this->getUpperBound(p_color); ++i)
	{
		for (auto &elem : this->m_piece_ptrs[x][i])
		{
			if (elem->getSideUp() == game::Piece::PAWN)
				return true;
		}
	}

	return false;
}

// Calculates number of occupiable spaces in specified file within specified color's territory
int Board::openings(game::Piece::Color p_color, int x)
{
	int count = 0;

	for (int i = this->getLowerBound(p_color); i <= this->getUpperBound(p_color); ++i)
	{
		int height = this->getHeight(x, i);

		if (height == MAX_HEIGHT)
			continue;

		if (height && this->m_piece_ptrs[x][i][height - 1]->getSideUp() == game::Piece::COMMANDER)
			continue;

		count += MAX_HEIGHT - height;
	}

	return count;
}

// Calculates number of fully occupied towers in specified file within specified color's territory
int Board::fullTowers(game::Piece::Color p_color, int x)
{
	int count = 0;

	for (int i = this->getLowerBound(p_color); i <= this->getUpperBound(p_color); ++i)
	{
		if (this->getHeight(x, i) == MAX_HEIGHT)
			++count;
	}

	return count;
}

glm::vec3 Board::getWorldCoords(game::Square *p_square_ptr, int z)
{
	if (p_square_ptr == nullptr)
		return SKIP_ROTATION;
	
	glm::vec3 coords;

	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	switch (p_square_ptr->getLocation())
	{
	case game::Square::BLACK_HAND:
		coords.x = 1.85f + 0.2f * (static_cast<float>(x) - 4.0f);
		coords.y = -0.09f + 0.015f * static_cast<float>(z);
		coords.z = 0.6f + 0.2f * (static_cast<float>(y) - 4.0f);

		break;

	case game::Square::WHITE_HAND:
		coords.x = -1.85f - 0.2f * (static_cast<float>(x) - 4.0f);
		coords.y = -0.09f + 0.015f * static_cast<float>(z);
		coords.z = -0.6f - 0.2f * (static_cast<float>(y) - 4.0f);

		break;

	case game::Square::BOARD:
		float y_off = 0.0f;

		for (int i = 1; i <= z; ++i)
			y_off += (this->getPiecePtr(x, y, i)->getSide() != this->getPiecePtr(x, y, i - 1)->getSide() ? 0.02f : 0.015f);

		coords.x = 0.0f + 0.2f * (static_cast<float>(x) - 4.0f);
		coords.y = 0.01f + y_off;
		coords.z = 0.0f + 0.2f * (static_cast<float>(y) - 4.0f);

		break;
	}

	return coords;
}

void Board::addAnimation(game::Square *p_src_square_ptr, game::Square *p_end_square_ptr, game::Square *p_mid_square_ptr, int z1, int z2, int z3)
{
	if (!this->m_animate)
		return;

	game::Piece *piece_ptr = this->getPiecePtr(p_src_square_ptr, z1);

	glm::vec3 src_coords = this->getWorldCoords(p_src_square_ptr, z1);
	glm::vec3 end_coords = this->getWorldCoords(p_end_square_ptr, z2);
	glm::vec3 mid_coords = this->getWorldCoords(p_mid_square_ptr, z3);

	this->endAnimation(piece_ptr);
	this->m_animations.push_back({ piece_ptr, src_coords, end_coords, mid_coords });
}

void Board::addAnimation(game::Square *p_src_square_ptr, game::Square *p_end_square_ptr, int z1, int z2)
{
	if (!this->m_animate)
		return;

	Hand *hand_ptr = this->getHandPtr(this->getActiveColor(*this->m_turn_ptr));
	game::Piece *piece_ptr = (p_src_square_ptr->getLocation() == game::Square::BOARD ? this->getPiecePtr(p_src_square_ptr, z1) : hand_ptr->getPiecePtr(p_src_square_ptr, z1));

	glm::vec3 src_coords = this->getWorldCoords(p_src_square_ptr, z1);
	glm::vec3 end_coords = this->getWorldCoords(p_end_square_ptr, z2);

	this->endAnimation(piece_ptr);
	this->m_animations.push_back({ piece_ptr, src_coords, end_coords });
}

void Board::addAnimation(game::Square *p_square_ptr, int z)
{
	if (!this->m_animate)
		return;

	game::Piece *piece_ptr = this->getPiecePtr(p_square_ptr, z);
	glm::vec3 coords = this->getWorldCoords(p_square_ptr, z);

	this->endAnimation(piece_ptr);
	this->m_animations.push_back({ piece_ptr, coords });
}

void Board::endAnimation(game::Piece *p_piece_ptr)
{
	for (auto i = this->m_animations.begin(); i != this->m_animations.end();)
	{
		if (this->m_animations[i - this->m_animations.begin()].getPiecePtr() == p_piece_ptr)
			i = this->m_animations.erase(i);

		else
			++i;
	}
}