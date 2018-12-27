/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Player.h
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

#ifndef PLAYER_H
#define PLAYER_H

#include "Game/Board.h"
#include "Game/Hand.h"

#define HUMAN 0
#define CHECKMATE 1000000000

struct Coords3D
{
	int x;
	int y;
	int z;
};

class Player
{
public:
	enum Function { SET, MOVE, STRIKE, DOWN, UP, EXCHANGE, SUBSTITUTE, };

	struct Move
	{
		Function func;

		Coords3D src;
		Coords3D dest;
		Coords3D rear;

		int score;
	};

	// Class functions
	// ---------------
	// Constructor
	Player(game::Piece::Color p_color, int *p_turn_ptr, Hand *p_black_hand_ptr, Hand *p_white_hand_ptr, Board *p_board_ptr);
	
	// Member functions
	// ----------------
	inline int getLevel() { return this->m_level; }

	inline bool evaluating() { return this->m_eval; }
	inline bool ready() { return this->m_ready; }

	inline bool controllable() { return (this->m_level == HUMAN); }

	void init(int p_level);

	void eval();
	bool act();

private:
	inline Hand* getHandPtr(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? this->m_white_hand_ptr : this->m_black_hand_ptr); }

	inline game::Piece::Color getActiveColor(int p_turn) { return (p_turn % 2 ? game::Piece::BLACK : game::Piece::WHITE); }
	inline game::Piece::Color getPassiveColor(int p_turn) { return (p_turn % 2 ? game::Piece::WHITE : game::Piece::BLACK); }

	inline int getLowerBound(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? LOWER_BOUND : BLACK_TERRITORY); }
	inline int getUpperBound(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? WHITE_TERRITORY : UPPER_BOUND); }

	void place1();
	void place2();
	void place3();

	void genPlacements(game::Square *p_square_ptr, const std::vector<game::Square*> &p_square_ptrs_ref);

	void move();

	void rearrange(Hand *p_hand_ptr, Move p_move);

	void genRearrangements(Move p_move, std::vector<Move> &p_moves_ref, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr);

	void actSim(Move p_move, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr);

	int minimax(int p_alpha, int p_beta, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr);

	int evalMaterial(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr);
	int evalMobility(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr);

	std::vector<Move> getMoves(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, Board *p_board_ptr);

	std::vector<game::Square*> getPlaceSquarePtrs(game::Piece *p_piece_ptr, int x, int y, bool p_stack);

	std::vector<game::Square*> getMRESquarePtrs(game::Piece *p_piece_ptr);

	std::vector<game::Square*> getPawnSquarePtrs(game::Piece *p_piece_ptr);
	std::vector<game::Square*> getPawnSquarePtrs(game::Piece *p_piece_ptr, int y);

	void reduceCatapultSquarePtrs(std::vector<game::Square*> &p_square_ptrs_ref, int y);
	void reduceFortressSquarePtrs(std::vector<game::Square*> &p_square_ptrs_ref, int y);
	
	// Member variables
	// ----------------
	game::Piece::Color m_color;

	int m_level = HUMAN;

	bool m_eval;
	bool m_ready;

	std::vector<Move> m_moves;
	
	// State references
	int *m_turn_ptr;

	Hand *m_black_hand_ptr;
	Hand *m_white_hand_ptr;

	Board *m_board_ptr;
};

#endif // PLAYER_H
