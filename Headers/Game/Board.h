/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Board.h
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

#ifndef BOARD_H
#define BOARD_H

#include "Game/Hand.h"
#include "Game/MRE.h"
#include "World/Animation.h"

#define MAX_HEIGHT 3
#define INITIAL_ARRANGEMENT 46

struct Action
{
	game::Square::Color color;
	game::Square::Color moves[BOARD_COLS][BOARD_ROWS];

	bool selected;
};

struct Selection
{
	game::Piece *piece_ptr;
	bool settable[BOARD_COLS][BOARD_ROWS];
};

struct Exchange
{
	game::Square *square_ptr;
	int turn;
};

class Board
{
public:
	// Class functions
	// ---------------
	// Constructor
	Board(int *p_turn_ptr, game::Piece **p_piece_pp, game::Square **p_square_pp, Hand *p_black_hand_ptr, Hand *p_white_hand_ptr);

	// Copy constructor
	Board(const Board &p_board_ref);

	// Member functions
	// ----------------
	inline std::vector<Exchange>& getExchangesRef() { return this->m_exchanges; }
	inline std::vector<Animation>* getAnimationsPtr() { return &this->m_animations; }

	inline bool& getBlackCheckRef() { return this->m_black_check; }
	inline bool& getWhiteCheckRef() { return this->m_white_check; }

	inline bool& getCheckRef(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? this->m_white_check : this->m_black_check); }

	inline bool& getBlackCheckmateRef() { return this->m_black_checkmate; }
	inline bool& getWhiteCheckmateRef() { return this->m_white_checkmate; }

	inline bool& getCheckmateRef(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? this->m_white_checkmate : this->m_black_checkmate); }

	inline bool& getAnimateRef() { return this->m_animate; }

	inline std::vector<game::Piece*>& getStackRef(int x, int y) { return this->m_piece_ptrs[x][y]; }
	inline std::vector<game::Piece*>& getStackRef(game::Square *p_square_ptr) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()]; }

	inline game::Piece* getPiecePtr(int x, int y, int z) { return this->m_piece_ptrs[x][y][z]; }
	inline game::Piece* getPiecePtr(game::Square *p_square_ptr, int z) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()][z]; }

	inline game::Piece* getPiecePtr(int x, int y) { return this->m_piece_ptrs[x][y].back(); } // Returns pointer to topmost piece at specified coordinates
	inline game::Piece* getPiecePtr(game::Square *p_square_ptr) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()].back(); } // Returns pointer to topmost piece at specified square

	inline game::Square* getSquarePtr(int x, int y) { return this->m_square_ptrs[x][y]; }

	inline int getHeight(int x, int y) { return this->m_piece_ptrs[x][y].size(); }
	inline int getHeight(game::Square *p_square_ptr) { return this->m_piece_ptrs[p_square_ptr->getX()][p_square_ptr->getY()].size(); }

	inline bool checkmate(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr) { return (this->checkmate(p_turn, p_active_hand_ptr, p_passive_hand_ptr, false) && this->checkmate(p_turn, p_active_hand_ptr, p_passive_hand_ptr, true)); }

	void init();
	void build(Set &p_set_ref); // Sets squares in their relative positions

	void clear();

	game::Piece* getMREPiecePtr(int x, int y); // Returns pointer to MRE imparting piece in tower at specified coordinates

	game::Square* getSelSquarePtr(); // Returns pointer to currently selected square
	game::Square* getCommSquarePtr(game::Piece::Color p_color); // Returns pointer to square containing commander of specified color

	void setPiecePtr(game::Piece *p_piece_ptr, int x, int y, int z);
	void setPiecePtr(game::Piece *p_piece_ptr, game::Square *p_square_ptr, int z);

	void setPiecePtr(game::Piece *p_piece_ptr, int x, int y); // Sets specified piece on top of stack at specified coordinates
	void setPiecePtr(game::Piece *p_piece_ptr, game::Square *p_square_ptr); // Sets specified piece on top of stack at specified square

	void insertPiecePtr(game::Piece *p_piece_ptr, int x, int y, int z); // Inserts specified piece in stack at specified coordinates at specified index
	void insertPiecePtr(game::Piece *p_piece_ptr, game::Square *p_square_ptr, int z); // Inserts specified piece in stack at specified square at specified index

	void flipPiecePtr(int x, int y, int z);
	void flipPiecePtr(game::Square *p_square_ptr, int z);

	void removePiecePtr(int x, int y, int z);
	void removePiecePtr(game::Square *p_square_ptr, int z);

	void removePiecePtr(int x, int y); // Removes topmost piece at specified coordinates
	void removePiecePtr(game::Square *p_square_ptr); // Removes topmost piece at specified square

	void removeAll(); // Removes all pieces

	void clear(game::Square::Color p_color); // Clears all squares of specified color
	void clearAll(); // Clears all squares

	void clearActions();
	void clearExchanges(int p_turn);

	std::vector<Move> getMoves(int x, int y); // Returns set of all in bound moves of topmost piece at specified coordinates

	void setPlaceable(); // Sets color of all squares current piece can place into
	void setDroppable(); // Sets color of all squares current piece can drop into
	void setRearrangeable(); // Sets color of all squares previously captured MRE piece can occupy

	void mouseOver();

	void mouseButton1();
	void mouseButton2();

	void mouseButton4();
	void mouseButton5();

	void set(int x1, int y1, int x2, int y2, Hand *p_hand_ptr);

	void move(int x1, int y1, int x2, int y2, Hand *p_hand_ptr);
	void strike(int x1, int y1, int x2, int y2, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	void strikeDown(int x, int y, int z, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void strikeUp(int x, int y, int z, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	void exchange(int x, int y, Hand *p_hand_ptr);
	void substitute(int x, int y);

	void setSim(game::Piece *p_piece_ptr, int x, int y, Hand *p_hand_ptr);

	void moveSim(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void strikeSim(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	void strikeDownSim(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void strikeUpSim(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	void exchangeSim(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void substituteSim(int x, int y);

	void betrayalSim(int x, int y, int p_turn);

	void recoverSim(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void recoverSim(game::Piece::Color p_color, int x, Hand *p_hand_ptr);

	void switchHandsSim(Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void handleRecoverySim(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	bool rearrangeableLat(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	bool rearrangeableVert(int x, int y, int z1, int z2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	bool selectable(int x, int y, int p_turn); // Determines if topmost piece at specified coordinates is accessible on specified turn

	bool placeable(game::Piece *p_piece_ptr, int x, int y); // Determines if placing specified piece at specified coordinates is valid
	bool droppable(game::Piece *p_piece_ptr, int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if dropping specified piece at specified coordinates is valid

	bool moveable(int x1, int y1, int x2, int y2); // Determines if move between specified coordinates is possible
	bool moveable(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if moving between specified coordinates is valid

	bool strikeable(int x1, int y1, int x2, int y2, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if striking between specified coordinates is valid

	bool exchangeable(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if 1-3 tier exchange is possible at specified coordinates on specified turn
	bool substitutable(int x, int y, int p_turn); // Determines if substitution is possible at specified coordinates on specified turn

	bool downwards(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if striking downwards at specified coordinates is valid
	bool upwards(int x, int y, int z, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if striking upwards at specified coordinates is valid

	bool check(game::Piece::Color p_color); // Determines if specified color is in check

private:
	inline Hand* getHandPtr(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? this->m_white_hand_ptr : this->m_black_hand_ptr); }

	inline game::Piece::Color getActiveColor(int p_turn) { return (p_turn % 2 ? game::Piece::BLACK : game::Piece::WHITE); }
	inline game::Piece::Color getPassiveColor(int p_turn) { return (p_turn % 2 ? game::Piece::WHITE : game::Piece::BLACK); }

	inline int getLowerBound(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? LOWER_BOUND : BLACK_TERRITORY); }
	inline int getUpperBound(game::Piece::Color p_color) { return (p_color == game::Piece::WHITE ? WHITE_TERRITORY : UPPER_BOUND); }
	
	void setMoveable(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Sets color of all squares in range of topmost piece at specified coordinates
	void setMRERange(int x, int y); // Sets color of all squares in range of MRE imparting piece in tower at specified coordinates

	void MB1Green(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void MB1Blue(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void MB1Red(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	void MB2Blue(int x, int y, Hand *p_hand_ptr);
	void MB2Red(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);

	void select(int x, int y, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr);
	void deselect();

	void betrayal(int x, int y, int z);

	void recover(int x, int y);
	void recover(game::Piece::Color p_color, int x);

	void transferLat(int x1, int y1, int x2, int y2);
	void transferVert(int x, int y, int z1, int z2);

	void remove(int x, int y, int z, bool p_flip, Hand *p_hand_ptr);

	void removeLat(int x1, int y1, int x2, int y2, Hand *p_hand_ptr);
	void removeVert(int x, int y, int z1, int z2, Hand *p_hand_ptr);

	void endTurn();

	bool stackable(game::Piece *p_piece_ptr, int x, int y); // Determines if specified piece is stackable at specified coordinates

	bool strikeable(int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if immobile strike is possible at specified coordinates on specified turn

	bool rearrangeable(game::Piece *p_piece_ptr, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if specified piece has at least one droppable square in own territory

	bool recoverable(game::Piece *p_piece_ptr, int x, int y, bool p_remove);
	bool recoverable(int x, int y, int z);

	bool recoverable(int x, int y); // Determines if topmost piece at specified coordinates is immoveable
	bool recoverable(game::Piece::Color p_color, int x, int y); // Determines if topmost piece at specified coordinates is immoveable as a result of MRE of specified color being removed

	bool checkmate(int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr, bool p_deferred); // Determines if active color on specified turn is checkmated

	bool dropLeavesInCheck(game::Piece *p_piece_ptr, int x, int y); // Determines if dropping specified piece at specified coordinates leaves in check
	bool dropCheckmates(game::Piece *p_piece_ptr, int x, int y, int p_turn, Hand *p_active_hand_ptr, Hand *p_passive_hand_ptr); // Determines if dropping specified piece at specified coordinates attains checkmate

	bool blocked(int x1, int y1, int x2, int y2);
	bool MREBlocked(game::Piece::Color p_alignment, int x, int y); // Determines if topmost piece at specified coordinates is not aligned and is within own MRE

	bool territoryFull(game::Piece::Color p_color);

	bool contains(game::Piece::Face p_face, int x, int y); // Determines if tower at specified coordinates contains piece of specified face
	bool contains(game::Piece::Face p_face, game::Square *p_square_ptr); // Determines if tower at specified square contains piece of specified face

	bool contains(game::Piece *p_piece_ptr, int x, int y); // Determines if tower at specified coordinates contains specified piece
	bool contains(game::Piece *p_piece_ptr, int x); // Determines if specified file contains specified piece

	bool containsPawn(game::Piece::Color p_color, int x); // Determines if specified file contains pawn within specified color's territory

	int openings(game::Piece::Color p_color, int x); // Calculates number of occupiable spaces in specified file within specified color's territory
	int fullTowers(game::Piece::Color p_color, int x); // Calculates number of fully occupied towers in specified file within specified color's territory

	glm::vec3 getWorldCoords(game::Square *p_square_ptr, int z);

	void addAnimation(game::Square *p_src_square_ptr, game::Square *p_end_square_ptr, game::Square *p_mid_square_ptr, int z1, int z2, int z3);
	void addAnimation(game::Square *p_src_square_ptr, game::Square *p_end_square_ptr, int z1, int z2);
	void addAnimation(game::Square *p_square_ptr, int z);

	void endAnimation(game::Piece *p_piece_ptr);

	// Member variables
	// ----------------
	std::vector<game::Piece*> m_piece_ptrs[BOARD_COLS][BOARD_ROWS];
	game::Square *m_square_ptrs[BOARD_COLS][BOARD_ROWS];
	Action m_actions[BOARD_COLS][BOARD_ROWS];

	std::vector<Selection> m_selections;
	std::vector<Exchange> m_exchanges;
	std::vector<Animation> m_animations;
	std::vector<game::Piece> m_temp_pieces;

	MRE m_MRE;

	bool m_black_check;
	bool m_white_check;

	bool m_black_checkmate;
	bool m_white_checkmate;

	bool m_animate;

	// Game state references
	int *m_turn_ptr;

	game::Piece **m_curr_piece_pp;
	game::Square **m_curr_square_pp;

	Hand *m_black_hand_ptr;
	Hand *m_white_hand_ptr;
};

#endif // BOARD_H