/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Hand.h
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

#ifndef HAND_H
#define HAND_H

#include "Game/Set.h"
#include "Game/Square.h"

#define HAND_ROWS 6

class Hand
{
public:
	// Class functions
	// ---------------
	// Constructor
	Hand(game::Piece::Color p_color, game::Square::Location p_location, int *p_turn_ptr, game::Piece **p_piece_pp, game::Square **p_square_pp);
	
	// Member functions
	// ----------------
	inline std::vector<game::Piece*>& getStackRef(int x, int y) { return this->m_piece_ptrs[x][y]; }
	inline std::vector<game::Piece*>& getStackRef(game::Square *p_square_ptr) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()]; }

	inline game::Piece* getPiecePtr(int x, int y, int z) { return this->m_piece_ptrs[x][y][z]; }
	inline game::Piece* getPiecePtr(game::Square *p_square_ptr, int z) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()][z]; }

	inline game::Piece* getPiecePtr(int x, int y) { return this->m_piece_ptrs[x][y].back(); }
	inline game::Piece* getPiecePtr(game::Square *p_square_ptr) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()].back(); }

	inline game::Square* getSquarePtr(int x, int y) { return this->m_square_ptrs[x][y]; }

	inline int getHeight(int x, int y) { return this->m_piece_ptrs[x][y].size(); }
	inline int getHeight(game::Square *p_square_ptr) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()].size(); }

	inline void add(game::Piece *p_piece_ptr, int x, int y) { this->m_piece_ptrs[x][y].push_back(p_piece_ptr); } // Sets specified piece on top of stack at specified coordinates
	inline void add(game::Piece *p_piece_ptr, game::Square *p_square_ptr) { this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()].push_back(p_piece_ptr); } // Sets specified piece on top of stack at specified square
	inline void add(game::Piece *p_piece_ptr) { if (game::Square *square_ptr = this->getSquarePtr(p_piece_ptr)) this->m_piece_ptrs[square_ptr->getX()][square_ptr->getY()].push_back(p_piece_ptr); }

	inline void remove(int x, int y) { this->m_piece_ptrs[x][y].pop_back(); } // Removes topmost piece at specified coordinates
	inline void remove(game::Square *p_square_ptr) { this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()].pop_back(); } // Removes topmost piece at specified square

	inline bool accessible(int p_turn) { return (this->m_color == (p_turn % 2 ? game::Piece::BLACK : game::Piece::WHITE)); }

	void init(Set &p_set_ref); // Sets pieces in their initial positions
	void build(Set &p_set_ref); // Sets squares in their relative positions

	std::vector<game::Piece*> getPiecePtrs();

	game::Piece* getMREPiecePtr(); // For forced rearrangement

	game::Square* getSquarePtr(game::Piece *p_piece_ptr);
	game::Square* getSelSquarePtr(); // Returns pointer to currently selected square

	void remove(game::Piece *p_piece_ptr); // Removes one of specified piece
	void removeAll(); // Removes all pieces

	void clear(game::Square::Color p_color); // Clears all squares of specified color
	void clearAll(); // Clears all squares

	void setSwappable();

	void mouseOver();
	void mouseButton1();

	bool empty();
	bool constrained();

private:
	// Member variables
	// ----------------
	std::vector<game::Piece*> m_piece_ptrs[HAND_COLS][HAND_ROWS];
	game::Square *m_square_ptrs[HAND_COLS][HAND_ROWS];

	game::Piece::Color m_color;
	game::Square::Location m_location;

	// Game state references
	int *m_turn_ptr;

	game::Piece **m_curr_piece_pp;
	game::Square **m_curr_square_pp;
};

#endif // HAND_H
