/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Player.cpp
 *
 * Summary:	Handles the game's AI by pseudorandomly choosing from a set of
 *			heuristically determined moves based on the CPU's level
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

#include "Game/Player.h"

#include <algorithm>

extern int rand(int p_min, int p_max);

// Class functions
// ---------------
// Constructor
Player::Player(game::Piece::Color p_color, int *p_turn_ptr, Hand *p_black_hand_ptr, Hand *p_white_hand_ptr, Board *p_board_ptr)
{
	this->m_color = p_color;
	
	this->m_turn_ptr = p_turn_ptr;

	this->m_black_hand_ptr = p_black_hand_ptr;
	this->m_white_hand_ptr = p_white_hand_ptr;

	this->m_board_ptr = p_board_ptr;
}

// Member functions
// ----------------
void Player::init(int p_level)
{
	this->m_level = p_level;

	this->m_eval = false;
	this->m_ready = false;

	this->m_moves.clear();
}

void Player::eval()
{
	this->m_eval = true;

	if (*this->m_turn_ptr <= INITIAL_ARRANGEMENT)
	{
		switch (this->m_level)
		{
		case 1:
			this->place1();
			break;

		case 2:
		case 3:
		case 4:
		case 5:
			this->place2();
			break;

		case 6:
		case 7:
		case 8:
		case 9:
			this->place3();
			break;
		}
	}

	else
		this->move();

	this->m_ready = true;
}

bool Player::act()
{
	if (!this->m_ready)
		return false;

	game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);
	game::Piece::Color passive = this->getPassiveColor(*this->m_turn_ptr);

	Hand *active_hand_ptr = this->getHandPtr(active);
	Hand *passive_hand_ptr = this->getHandPtr(passive);

	Move move = this->m_moves[rand(0, this->m_moves.size() - 1)];

	switch (move.func)
	{
	case SET:
		this->m_board_ptr->set(move.src.x, move.src.y, move.dest.x, move.dest.y, active_hand_ptr);
		break;

	case MOVE:
		this->m_board_ptr->move(move.src.x, move.src.y, move.dest.x, move.dest.y, active_hand_ptr);
		break;

	case STRIKE:
		this->m_board_ptr->strike(move.src.x, move.src.y, move.dest.x, move.dest.y, active_hand_ptr, passive_hand_ptr);
		this->rearrange(active_hand_ptr, move);

		break;

	case DOWN:
		this->m_board_ptr->strikeDown(move.src.x, move.src.y, move.src.z, active_hand_ptr, passive_hand_ptr);
		this->rearrange(active_hand_ptr, move);

		break;

	case UP:
		this->m_board_ptr->strikeUp(move.src.x, move.src.y, move.src.z, active_hand_ptr, passive_hand_ptr);
		this->rearrange(active_hand_ptr, move);

		break;

	case EXCHANGE:
		this->m_board_ptr->exchange(move.src.x, move.src.y, active_hand_ptr);
		break;

	case SUBSTITUTE:
		this->m_board_ptr->substitute(move.src.x, move.src.y);
		break;
	}

	this->m_board_ptr->clear();

	++(*this->m_turn_ptr);

	if (*this->m_turn_ptr > INITIAL_ARRANGEMENT)
	{
		this->m_board_ptr->getBlackCheckRef() = this->m_board_ptr->check(game::Piece::BLACK);
		this->m_board_ptr->getWhiteCheckRef() = this->m_board_ptr->check(game::Piece::WHITE);
		
		if (!(this->m_board_ptr->getCheckmateRef(active) = this->m_board_ptr->getCheckRef(active)))
			this->m_board_ptr->getCheckmateRef(passive) = this->m_board_ptr->checkmate(*this->m_turn_ptr, passive_hand_ptr, active_hand_ptr);
	}

	this->m_eval = false;
	this->m_ready = false;

	this->m_moves.clear();

	return true;
}

void Player::place1()
{
	Hand *hand_ptr = this->getHandPtr(this->m_color);

	int lower_bound = this->getLowerBound(this->m_color);
	int upper_bound = this->getUpperBound(this->m_color);

	for (auto &elem : hand_ptr->getPiecePtrs())
	{
		if (game::Square *square_ptr = hand_ptr->getSquarePtr(elem))
		{
			int x1 = square_ptr->getX();
			int y1 = square_ptr->getY();
			
			for (int i = lower_bound; i <= upper_bound; ++i)
			{
				for (int j = 0; j < BOARD_COLS; ++j)
				{
					if (this->m_board_ptr->placeable(elem, j, i))
						this->m_moves.push_back({ SET, { x1, y1 }, { j, i } });
				}
			}
		}
	}
}

void Player::place2()
{
	Hand *hand_ptr = this->getHandPtr(this->m_color);

	int rank1 = (this->m_color == game::Piece::WHITE ? 0 : 8);
	int rank2 = (this->m_color == game::Piece::WHITE ? 1 : 7);
	int rank3 = (this->m_color == game::Piece::WHITE ? 2 : 6);

	for (auto &elem : hand_ptr->getPiecePtrs())
	{
		std::vector<game::Square*> square_ptrs;
		
		switch (elem->getSideUp())
		{
		case game::Piece::COMMANDER:
		case game::Piece::FORTRESS:
			square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank1, false);
			break;

		case game::Piece::CAPTAIN:
		case game::Piece::SAMURAI:
		case game::Piece::SPY:
			if ((square_ptrs = this->getPawnSquarePtrs(elem)).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank3, true);

			break;

		case game::Piece::CATAPULT:
			square_ptrs = this->getPlaceSquarePtrs(elem, 2, rank2, false);
			break;

		case game::Piece::HIDDEN_DRAGON:
		case game::Piece::PRODIGY:
			if ((square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank3, false)).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank2, false);

			break;

		case game::Piece::BOW:
			if ((square_ptrs = this->getMRESquarePtrs(elem)).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank2, true);

			break;

		case game::Piece::PAWN:
			if ((square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank3, true)).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank2, true);

			break;
		}

		if (game::Square *square_ptr = hand_ptr->getSquarePtr(elem))
			this->genPlacements(square_ptr, square_ptrs);
	}
}

void Player::place3()
{
	Hand *hand_ptr = this->getHandPtr(this->m_color);

	int rank1 = (this->m_color == game::Piece::WHITE ? 0 : 8);
	int rank2 = (this->m_color == game::Piece::WHITE ? 1 : 7);
	int rank3 = (this->m_color == game::Piece::WHITE ? 2 : 6);

	for (auto &elem : hand_ptr->getPiecePtrs())
	{
		std::vector<game::Square*> square_ptrs;

		switch (elem->getSideUp())
		{
		case game::Piece::COMMANDER:
			if (hand_ptr->getStackRef(3, 2).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank1, false);

			break;

		case game::Piece::CAPTAIN:
		case game::Piece::SAMURAI:
		case game::Piece::SPY:
			square_ptrs = this->getPawnSquarePtrs(elem, rank3);
			break;

		case game::Piece::CATAPULT:
			if (hand_ptr->getStackRef(2, 2).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 2, rank2, false);

			if (hand_ptr->getStackRef(1, 1).empty())
				this->reduceCatapultSquarePtrs(square_ptrs, rank1);

			break;

		case game::Piece::FORTRESS:
			if (hand_ptr->getStackRef(3, 2).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank1, false);

			if (hand_ptr->getStackRef(0, 1).empty())
				this->reduceFortressSquarePtrs(square_ptrs, rank2);

			break;

		case game::Piece::HIDDEN_DRAGON:
		case game::Piece::PRODIGY:
			if (hand_ptr->getStackRef(1, 2).empty())
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank3, false);

			break;

		case game::Piece::BOW:
			square_ptrs = this->getMRESquarePtrs(elem);
			break;

		case game::Piece::PAWN:
			switch (elem->getSideDown())
			{
			case game::Piece::BRONZE:
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank3, false);
				break;

			case game::Piece::SILVER:
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank2, false);
				break;

			case game::Piece::GOLD:
				square_ptrs = this->getPlaceSquarePtrs(elem, 0, rank1, false);
				break;
			}

			break;
		}

		if (game::Square *square_ptr = hand_ptr->getSquarePtr(elem))
			this->genPlacements(square_ptr, square_ptrs);
	}
}

void Player::genPlacements(game::Square *p_square_ptr, const std::vector<game::Square*> &p_square_ptrs_ref)
{
	int x1 = p_square_ptr->getX();
	int y1 = p_square_ptr->getY();

	for (auto &elem : p_square_ptrs_ref)
	{
		int x2 = elem->getX();
		int y2 = elem->getY();

		this->m_moves.push_back({ SET, { x1, y1 }, { x2, y2 } });
	}
}

void Player::move()
{
	game::Piece::Color active = this->getActiveColor(*this->m_turn_ptr);
	game::Piece::Color passive = this->getPassiveColor(*this->m_turn_ptr);

	Hand *active_hand_ptr = this->getHandPtr(active);
	Hand *passive_hand_ptr = this->getHandPtr(passive);

	this->m_moves = this->getMoves(*this->m_turn_ptr, active_hand_ptr, passive_hand_ptr, this->m_board_ptr);

	if (this->m_level < 2)
		return;

	int best = INT_MIN;

	for (auto &elem : this->m_moves)
	{
		Board temp_board = *this->m_board_ptr;

		Hand temp_active_hand = *active_hand_ptr;
		Hand temp_passive_hand = *passive_hand_ptr;

		this->actP(elem, *this->m_turn_ptr, &temp_active_hand, &temp_passive_hand, &temp_board);

		elem.score = this->minimax(INT_MIN, INT_MAX, *this->m_turn_ptr, &temp_active_hand, &temp_passive_hand, &temp_board);
		best = std::max(best, elem.score);
	}

	for (auto i = this->m_moves.begin(); i != this->m_moves.end();)
	{
		if (i->score < best)
			i = this->m_moves.erase(i);

		else
			++i;
	}
}

void Player::rearrange(Hand *p_hand_ptr, Move p_move)
{
	if (game::Piece *piece_ptr = p_hand_ptr->getMREPiecePtr())
	{
		if (game::Square *square_ptr = p_hand_ptr->getSquarePtr(piece_ptr))
		{
			int x = square_ptr->getX();
			int y = square_ptr->getY();

			this->m_board_ptr->set(x, y, p_move.rear.x, p_move.rear.y, p_hand_ptr);
		}
	}
}

void Player::genRearrangements(Move p_move, std::vector<Move> &p_moves_ref, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr)
{
	Board temp_board = *p_board_ptr;

	Hand temp_active_hand = *p_active_hand_ptr;
	Hand temp_passive_hand = *p_passive_hand_ptr;

	switch (p_move.func)
	{
	case STRIKE:
		temp_board.strikeSim(p_move.src.x, p_move.src.y, p_move.dest.x, p_move.dest.y, p_turn, &temp_active_hand, &temp_passive_hand);
		break;

	case DOWN:
		temp_board.strikeDownSim(p_move.src.x, p_move.src.y, p_move.src.z, p_turn, &temp_active_hand, &temp_passive_hand);
		break;

	case UP:
		temp_board.strikeUpSim(p_move.src.x, p_move.src.y, p_move.src.z, p_turn, &temp_active_hand, &temp_passive_hand);
		break;
	}

	if (game::Piece *piece_ptr = temp_active_hand.getMREPiecePtr())
	{
		game::Piece::Color active = this->getActiveColor(p_turn);

		int lower_bound = this->getLowerBound(active);
		int upper_bound = this->getUpperBound(active);
		
		for (int i = lower_bound; i <= upper_bound; ++i)
		{
			for (int j = 0; j < BOARD_COLS; ++j)
			{
				if (temp_board.droppable(piece_ptr, j, i, p_turn, &temp_active_hand, &temp_passive_hand))
					p_moves_ref.push_back({ p_move.func, p_move.src, p_move.dest, { j, i } });
			}
		}
	}
}

void Player::actP(Move p_move, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr)
{
	switch (p_move.func)
	{
	case SET:
		if (game::Piece *piece_ptr = p_active_hand_ptr->getPiecePtr(p_move.src.x, p_move.src.y))
			p_board_ptr->setSim(piece_ptr, p_move.dest.x, p_move.dest.y, p_active_hand_ptr);

		break;

	case MOVE:
		p_board_ptr->moveSim(p_move.src.x, p_move.src.y, p_move.dest.x, p_move.dest.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
		break;

	case STRIKE:
		p_board_ptr->strikeSim(p_move.src.x, p_move.src.y, p_move.dest.x, p_move.dest.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

		if (game::Piece *piece_ptr = p_active_hand_ptr->getMREPiecePtr())
			p_board_ptr->setSim(piece_ptr, p_move.rear.x, p_move.rear.y, p_active_hand_ptr);

		break;

	case DOWN:
		p_board_ptr->strikeDownSim(p_move.src.x, p_move.src.y, p_move.src.z, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

		if (game::Piece *piece_ptr = p_active_hand_ptr->getMREPiecePtr())
			p_board_ptr->setSim(piece_ptr, p_move.rear.x, p_move.rear.y, p_active_hand_ptr);

		break;

	case UP:
		p_board_ptr->strikeUpSim(p_move.src.x, p_move.src.y, p_move.src.z, p_turn, p_active_hand_ptr, p_passive_hand_ptr);

		if (game::Piece *piece_ptr = p_active_hand_ptr->getMREPiecePtr())
			p_board_ptr->setSim(piece_ptr, p_move.rear.x, p_move.rear.y, p_active_hand_ptr);

		break;

	case EXCHANGE:
		p_board_ptr->exchangeSim(p_move.src.x, p_move.src.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr);
		break;

	case SUBSTITUTE:
		p_board_ptr->substituteSim(p_move.src.x, p_move.src.y);
		break;
	}
}

int Player::minimax(int p_alpha, int p_beta, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr)
{
	int depth = p_turn - *this->m_turn_ptr;
	int divisor = static_cast<int>(std::pow(2, depth));

	int level_mod = this->m_level - 2;
	int depth_mod = depth * 4;

	if (level_mod > depth_mod + 2)
	{
		if (p_board_ptr->checkmate(p_turn + 1, p_passive_hand_ptr, p_active_hand_ptr))
			return (CHECKMATE / divisor);
	}

	int bottom = level_mod / 4;

	if (depth == bottom)
	{
		int score = this->evalMaterial(p_turn, p_active_hand_ptr, p_passive_hand_ptr, p_board_ptr);

		if (level_mod > depth_mod)
			score -= this->evalMobility(p_turn + 1, p_passive_hand_ptr, p_active_hand_ptr, p_board_ptr);

		if (level_mod > depth_mod + 1)
			score += this->evalMobility(p_turn + 2, p_active_hand_ptr, p_passive_hand_ptr, p_board_ptr);

		return score;
	}

	std::vector<Move> moves = this->getMoves(p_turn + 1, p_passive_hand_ptr, p_active_hand_ptr, p_board_ptr);

	int best = 0;
	int sign = 0;

	if ((depth + 1) % 2)
	{
		best = INT_MAX;
		sign = -1;
	}

	else
	{
		best = INT_MIN;
		sign = 1;
	}

	for (auto &elem : moves)
	{
		Board temp_board = *p_board_ptr;

		Hand temp_active_hand = *p_active_hand_ptr;
		Hand temp_passive_hand = *p_passive_hand_ptr;

		this->actP(elem, p_turn + 1, &temp_passive_hand, &temp_active_hand, &temp_board);

		elem.score = sign * this->minimax(p_alpha, p_beta, p_turn + 1, &temp_passive_hand, &temp_active_hand, &temp_board);

		if ((depth + 1) % 2)
		{
			best = std::min(best, elem.score);
			p_beta = std::min(p_beta, best);
		}

		else
		{
			best = std::max(best, elem.score);
			p_alpha = std::max(p_alpha, best);
		}

		if (p_beta <= p_alpha)
			break;
	}

	return best;
}

int Player::evalMaterial(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr)
{
	int score = 0;
	
	game::Piece::Color active = this->getActiveColor(p_turn);

	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			for (auto &elem : p_board_ptr->getStackRef(j, i))
				elem->getAlignment() == active ? score += elem->getWeight() : score -= elem->getWeight();
		}
	}

	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			for (auto &elem : p_active_hand_ptr->getStackRef(j, i))
				score += elem->getWeight() / 2;

			for (auto &elem : p_passive_hand_ptr->getStackRef(j, i))
				score -= elem->getWeight() / 2;
		}
	}

	return score;
}

int Player::evalMobility(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr)
{
	int score = 0;
	int best = 0;

	int depth = p_turn - *this->m_turn_ptr;
	int divisor = static_cast<int>(std::pow(2, depth));

	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (p_board_ptr->selectable(j, i, p_turn))
			{
				for (auto &elem : p_board_ptr->getMoves(j, i))
				{
					if (!p_board_ptr->moveable(j, i, elem.x, elem.y))
						continue;

					if (p_board_ptr->strikeable(j, i, elem.x, elem.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
						best = std::max(best, p_board_ptr->getPiecePtr(elem.x, elem.y)->getWeight());

					++score;
				}
			}

			int height = p_board_ptr->getHeight(j, i);

			for (int k = 0; k < height; ++k)
			{
				if (k > 0 && p_board_ptr->downwards(j, i, k, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
					best = std::max(best, p_board_ptr->getPiecePtr(j, i, k - 1)->getWeight());;

				if (k < height - 1 && p_board_ptr->upwards(j, i, k, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
					best = std::max(best, p_board_ptr->getPiecePtr(j, i, k + 1)->getWeight());
			}
		}
	}

	score += p_active_hand_ptr->getPiecePtrs().size();

	return ((score + best) / divisor);
}

std::vector<Player::Move> Player::getMoves(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr)
{
	std::vector<Move> moves;

	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			for (auto &elem : p_active_hand_ptr->getPiecePtrs())
			{
				if (p_board_ptr->droppable(elem, j, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
				{
					if (game::Square *square_ptr = p_active_hand_ptr->getSquarePtr(elem))
						moves.push_back({ SET, { square_ptr->getX(), square_ptr->getY() }, { j, i } });
				}
			}

			if (p_board_ptr->selectable(j, i, p_turn))
			{
				for (auto &elem : p_board_ptr->getMoves(j, i))
				{
					if (!p_board_ptr->moveable(j, i, elem.x, elem.y))
						continue;

					if (p_board_ptr->moveable(j, i, elem.x, elem.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
						moves.push_back({ MOVE, { j, i }, { elem.x, elem.y } });

					if (p_board_ptr->strikeable(j, i, elem.x, elem.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
					{
						if (p_board_ptr->rearrangeableLat(j, i, elem.x, elem.y, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
							this->genRearrangements({ STRIKE, { j, i }, { elem.x, elem.y } }, moves, p_turn, p_active_hand_ptr, p_passive_hand_ptr, p_board_ptr);

						else
							moves.push_back({ STRIKE, { j, i }, { elem.x, elem.y } });
					}
				}
			}

			if (p_board_ptr->exchangeable(j, i, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
				moves.push_back({ EXCHANGE, { j, i } });

			if (p_board_ptr->substitutable(j, i, p_turn))
				moves.push_back({ SUBSTITUTE, { j, i } });

			int height = p_board_ptr->getHeight(j, i);

			if (height < 2)
				continue;

			for (int k = 0; k < height; ++k)
			{
				if (k > 0 && p_board_ptr->downwards(j, i, k, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
				{
					if (p_board_ptr->rearrangeableVert(j, i, k, k - 1, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
						this->genRearrangements({ DOWN, { j, i, k } }, moves, p_turn, p_active_hand_ptr, p_passive_hand_ptr, p_board_ptr);

					else
						moves.push_back({ DOWN, { j, i, k } });
				}

				if (k < height - 1 && p_board_ptr->upwards(j, i, k, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
				{
					if (p_board_ptr->rearrangeableVert(j, i, k, k + 1, p_turn, p_active_hand_ptr, p_passive_hand_ptr))
						this->genRearrangements({ UP, { j, i, k } }, moves, p_turn, p_active_hand_ptr, p_passive_hand_ptr, p_board_ptr);

					else
						moves.push_back({ UP, { j, i, k } });
				}
			}
		}
	}

	return moves;
}

std::vector<game::Square*> Player::getPlaceSquarePtrs(game::Piece *p_piece_ptr, int x, int y, bool p_stack)
{
	std::vector<game::Square*> square_ptrs;
	
	for (int i = 0 + x; i < BOARD_COLS - x; ++i)
	{
		int height = this->m_board_ptr->getHeight(i, y);

		if (height == 0 || p_stack)
		{
			if (this->m_board_ptr->placeable(p_piece_ptr, i, y))
				square_ptrs.push_back(this->m_board_ptr->getSquarePtr(i, y));
		}
	}

	return square_ptrs;
}

std::vector<game::Square*> Player::getMRESquarePtrs(game::Piece *p_piece_ptr)
{
	std::vector<game::Square*> square_ptrs;

	for (int i = this->getLowerBound(this->m_color); i <= this->getUpperBound(this->m_color); ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->m_board_ptr->getHeight(j, i) && this->m_board_ptr->getPiecePtr(j, i)->impartsMRE())
			{
				if (this->m_board_ptr->placeable(p_piece_ptr, j, i))
					square_ptrs.push_back(this->m_board_ptr->getSquarePtr(j, i));
			}
		}
	}

	return square_ptrs;
}

std::vector<game::Square*> Player::getPawnSquarePtrs(game::Piece *p_piece_ptr)
{
	std::vector<game::Square*> square_ptrs;

	for (int i = this->getLowerBound(this->m_color); i <= this->getUpperBound(this->m_color); ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			if (this->m_board_ptr->getHeight(j, i) && this->m_board_ptr->getPiecePtr(j, i)->getSideUp() == game::Piece::PAWN)
			{
				if (this->m_board_ptr->placeable(p_piece_ptr, j, i))
					square_ptrs.push_back(this->m_board_ptr->getSquarePtr(j, i));
			}
		}
	}

	return square_ptrs;
}

std::vector<game::Square*> Player::getPawnSquarePtrs(game::Piece *p_piece_ptr, int y)
{
	std::vector<game::Square*> square_ptrs;

	for (int i = 0; i < BOARD_COLS; ++i)
	{
		if (this->m_board_ptr->getHeight(i, y) && this->m_board_ptr->getPiecePtr(i, y)->getSideUp() == game::Piece::PAWN)
		{
			if (this->m_board_ptr->placeable(p_piece_ptr, i, y))
				square_ptrs.push_back(this->m_board_ptr->getSquarePtr(i, y));
		}
	}

	return square_ptrs;
}

void Player::reduceCatapultSquarePtrs(std::vector<game::Square*> &p_square_ptrs_ref, int y)
{
	int x1 = 0;

	for (int i = 0; i < BOARD_COLS; ++i)
	{
		for (auto &elem : this->m_board_ptr->getStackRef(i, y))
		{
			if (elem->getSideUp() == game::Piece::FORTRESS)
				x1 = i;
		}
	}

	if (x1 < 2 || x1 > 6)
		return;

	for (auto i = p_square_ptrs_ref.begin(); i != p_square_ptrs_ref.end();)
	{
		int x2 = p_square_ptrs_ref[i - p_square_ptrs_ref.begin()]->getX();

		if (x1 >= x2 - 1 && x1 <= x2 + 1)
			i = p_square_ptrs_ref.erase(i);

		else
			++i;
	}
}

void Player::reduceFortressSquarePtrs(std::vector<game::Square*> &p_square_ptrs_ref, int y)
{
	int x1 = 0;

	for (int i = 2; i < BOARD_COLS - 2; ++i)
	{
		for (auto &elem : this->m_board_ptr->getStackRef(i, y))
		{
			if (elem->getSideUp() == game::Piece::CATAPULT)
				x1 = i;
		}
	}

	for (auto i = p_square_ptrs_ref.begin(); i != p_square_ptrs_ref.end();)
	{
		int x2 = p_square_ptrs_ref[i - p_square_ptrs_ref.begin()]->getX();

		if (x2 >= x1 - 2 && x2 <= x1 + 2)
			i = p_square_ptrs_ref.erase(i);

		else
			++i;
	}
}