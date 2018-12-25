/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	State.h
 *
 * Summary:	Manages all of the objects that collectively act as the logical
 *			representation of the game as well as handle saving and loading
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

#ifndef STATE_H
#define STATE_H

#include "Game/Player.h"
#include "Game/Position.h"

#define MOUSE_BUTTON_1 0
#define MOUSE_BUTTON_2 1

#define MOUSE_BUTTON_4 3
#define MOUSE_BUTTON_5 4

#define STALEMATE 4

class State
{
public:
	// Member functions
	// ----------------
	inline int getTurn() { return this->m_turn; }

	inline game::Piece* getCurrPiecePtr() { return this->m_curr_piece_ptr; }
	inline game::Square* getCurrSquarePtr() { return this->m_curr_square_ptr; }

	inline Set* getSetPtr() { return &this->m_set; }

	inline Hand* getBlackHandPtr() { return &this->m_black_hand; }
	inline Hand* getWhiteHandPtr() { return &this->m_white_hand; }

	inline Hand* getHandPtr(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? &this->m_white_hand : &this->m_black_hand); }

	inline Board* getBoardPtr() { return &this->m_board; }

	inline Player* getBlackPlayerPtr() { return &this->m_black_player; }
	inline Player* getWhitePlayerPtr() { return &this->m_white_player; }

	inline Player* getActivePlayerPtr() { return (this->getActiveColor(this->m_turn) == game::Piece::WHITE ? &this->m_white_player : &this->m_black_player); }
	inline Player* getPassivePlayerPtr() { return (this->getPassiveColor(this->m_turn) == game::Piece::WHITE ? &this->m_white_player : &this->m_black_player); }

	inline void setCurrSquarePtr(game::Square *p_square_ptr) { this->m_curr_square_ptr = p_square_ptr; }

	inline bool stalemate() { return this->m_stalemate; }
	inline bool gameOver() { return (this->m_stalemate || this->m_board.getWhiteCheckmateRef() || this->m_board.getBlackCheckmateRef()); }

	void init(int *p_settings_ptr = nullptr);
	void build();

	void mouseClick(int p_button);

	void handleHandMB1();
	bool handleAI();

	void save(const std::string &p_path_ref); // Record current game representation to file
	void load(const std::string &p_path_ref); // Restore previously recorded game from file

private:
	inline Player* getPlayerPtr(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? &this->m_white_player : &this->m_black_player); }

	inline game::Piece::Color getActiveColor(int p_turn) { return (p_turn % 2 ? game::Piece::BLACK : game::Piece::WHITE); }
	inline game::Piece::Color getPassiveColor(int p_turn) { return (p_turn % 2 ? game::Piece::WHITE : game::Piece::BLACK); }

	void updatePositions();

	std::string XOR(const std::string &p_line_ref); // For encryption and decryption of game files

	// Member variables
	// ----------------
	int m_turn = 1;
	bool m_stalemate;

	game::Piece *m_curr_piece_ptr; // Currently selected piece (left clicked)
	game::Square *m_curr_square_ptr; // Currently selected square (moused over)

	Set m_set; // Set of all game pieces and game squares

	// Hands
	Hand m_black_hand = Hand(game::Piece::BLACK, game::Square::BLACK_HAND, &this->m_turn, &this->m_curr_piece_ptr, &this->m_curr_square_ptr);
	Hand m_white_hand = Hand(game::Piece::WHITE, game::Square::WHITE_HAND, &this->m_turn, &this->m_curr_piece_ptr, &this->m_curr_square_ptr);

	// Board
	Board m_board = Board(&this->m_turn, &this->m_curr_piece_ptr, &this->m_curr_square_ptr, &this->m_black_hand, &this->m_white_hand);
	std::vector<Position> m_positions; // Set of all board positions to have occurred post initial arrangement

	// Players
	Player m_black_player = Player(game::Piece::BLACK, &this->m_turn, &this->m_black_hand, &this->m_white_hand, &this->m_board);
	Player m_white_player = Player(game::Piece::WHITE, &this->m_turn, &this->m_black_hand, &this->m_white_hand, &this->m_board);
};

#endif // STATE_H