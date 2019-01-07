/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Piece.h
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

#ifndef GAME_PIECE_H
#define GAME_PIECE_H

#include <vector>

#define LOWER_BOUND 0
#define UPPER_BOUND 8

// Weight values corresponding to piece front and back faces
static const int WEIGHTS[] =
{
	0,
	0, 700, 550, 450, 900, 850, 1000, 950, 750, 100, 150, 200,
	300, 350, 800, 650, 600, 1100, 1050, 400, 50, 250, 500,
};

// String literals corresponding to face identifiers
static const std::string FACE_STRINGS[] =
{
	"",
	"Commander",  "Captain",  "Samurai",  "Spy",  "Catapult",  "Fortress",  "Hidden Dragon",  "Prodigy",  "Bow",  "Pawn",
	"Pistol",  "Pike",  "Clandestinite",  "Lance",  "Dragon King",  "Phoenix",  "Arrow",  "Bronze",  "Silver",  "Gold",
};

struct Move
{
	int x;
	int y;
};

namespace game { class Piece; }

class game::Piece
{
public:
	enum Face
	{
		BLANK,
		COMMANDER, CAPTAIN, SAMURAI, SPY, CATAPULT, FORTRESS, HIDDEN_DRAGON, PRODIGY, BOW, PAWN,
		PISTOL, PIKE, CLANDESTINITE, LANCE, DRAGON_KING, PHOENIX, ARROW, BRONZE, SILVER, GOLD,
	};

	enum Color { BLACK = 0, WHITE = 23, };
	enum Side { FRONT = 1, BACK = 11, };

	// Member functions
	// ----------------
	inline void init() { this->m_side = FRONT; }

	inline int getID() { return this->m_ID; }

	inline Face getFront() { return this->m_front; }
	inline Face getBack() { return this->m_back; }

	inline Face getSideUp() { return (this->m_side == BACK ? this->m_back : this->m_front); }
	inline Face getSideDown() { return (this->m_side == BACK ? this->m_front : this->m_back); }

	inline Color getColor() { return this->m_color; }

	inline Color getAlignment() { return (this->m_side == BACK ? (this->m_color == WHITE ? BLACK : WHITE) : this->m_color); }
	inline Color getInverse() { return (this->m_color == WHITE ? BLACK : WHITE); }

	inline Side getSide() { return this->m_side; }

	inline std::string getFrontString() { return FACE_STRINGS[this->m_front]; }
	inline std::string getBackString() { return FACE_STRINGS[this->m_back]; }

	inline std::string getSideUpString() { return FACE_STRINGS[this->getSideUp()]; }
	inline std::string getSideDownString() { return FACE_STRINGS[this->getSideDown()]; }

	inline void flip() { this->m_side = (this->m_side == BACK ? FRONT : BACK); }

	inline bool jumps() { return (this->getSideUp() == SPY || this->getSideUp() == BOW || this->getSideUp() == CLANDESTINITE); }

	inline bool links() { return (this->getSideUp() == SPY || this->getSideUp() == CATAPULT || this->getSideUp() == FORTRESS || this->getSideUp() == CLANDESTINITE); }
	inline bool recovers() { return (this->getSideUp() == SPY || this->getSideUp() == PAWN || this->getSideUp() == LANCE); }

	inline bool impartsMRE() { return (this->getSideUp() == CATAPULT || this->getSideUp() == FORTRESS); }
	inline bool receivesMRE() { return !(this->getSideUp() == COMMANDER || this->getSideUp() == HIDDEN_DRAGON || this->getSideUp() == PRODIGY || this->getSideUp() == DRAGON_KING || this->getSideUp() == PHOENIX); }

	inline bool dropsFreely() { return !(this->getSideUp() == SPY || this->getSideUp() == PAWN || this->getSideUp() == LANCE || this->getSideUp() == BRONZE); }

	inline bool accessible(int p_turn) { return (this->getAlignment() == (p_turn % 2 ? BLACK : WHITE)); }

	void build(int p_ID, Face p_front, Face p_back, Color p_color);

	bool shallowEquals(Piece *p_piece_ptr);
	bool equals(Piece *p_piece_ptr);

	int getWeight();

	std::vector<Move> getMoves(int x, int y, int z);
	std::vector<Move> getGoldMoves(int x, int y);

private:
	inline void addMove(std::vector<Move> &p_moves_ref, int x, int y) { if (x >= LOWER_BOUND && x <= UPPER_BOUND && y >= LOWER_BOUND && y <= UPPER_BOUND) p_moves_ref.push_back({ x, y }); }

	std::vector<Move> getDiagonalMoves(int x, int y);
	std::vector<Move> getOrthogonalMoves(int x, int y);

	std::vector<Move> getExtendedDiagonalMoves(int x, int y);
	std::vector<Move> getExtendedOrthogonalMoves(int x, int y);

	// Member variables
	// ----------------
	int m_ID;

	Face m_front;
	Face m_back;

	Color m_color;

	Side m_side;
};

#endif // GAME_PIECE_H
