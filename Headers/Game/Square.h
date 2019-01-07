/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Square.h
 * 
 * Summary:	Maintains a logical representation of a game square
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

#ifndef GAME_SQUARE_H
#define GAME_SQUARE_H

#include <glm/glm.hpp>

#include <string>

#define HAND_COLS 4
#define BOARD_COLS 9

// RGBA values corresponding to color identifiers
static const glm::vec4 RGBA[] =
{
	glm::vec4(0.0f,   0.0f,   0.0f,   0.0f), // CLEAR
	glm::vec4(1.0f,   0.025f, 0.025f, 1.0f), // RED
	glm::vec4(0.0f,   0.5f,   0.1f,   1.0f), // GREEN
	glm::vec4(0.0f,   0.1f,   0.5f,   1.0f), // BLUE
	glm::vec4(0.25f,  0.1f,   0.5f,   1.0f), // PURPLE
	glm::vec4(0.5f,   0.5f,   0.5f,   1.0f), // GRAY
};

// String literals corresponding to color identifiers
static const std::string COLOR_STRINGS[] = { "Clear", "Red", "Green", "Blue", "Purple", "Gray", };

namespace game { class Square; }

class game::Square
{
public:
	enum Location { BLACK_HAND = 0, WHITE_HAND = 24, BOARD = 48, };
	enum Color { CLEAR, RED, GREEN, BLUE, PURPLE, GRAY, };

	// Member functions
	// ----------------
	inline void init() { this->m_color = CLEAR; }

	inline int getID() { return this->m_ID; }

	inline Location getLocation() { return this->m_location; }
	inline Color getColor() { return this->m_color; }

	inline glm::vec4 getRGBA() { return RGBA[this->m_color]; }
	inline std::string getColorString() { return COLOR_STRINGS[this->m_color]; }

	inline int getX() { return (this->m_ID - this->m_location) % this->getNumCols(); }
	inline int getY() { return (this->m_ID - this->m_location) / this->getNumCols(); }

	inline void setColor(Color p_color) { this->m_color = p_color; }

	inline bool interactable() { return (this->m_color == RED || this->m_color == GREEN || this->m_color == BLUE); }

	inline void build(int p_ID, Location p_location)
	{
		this->m_ID = p_ID;
		this->m_location = p_location;
	}

private:
	inline int getNumCols() { return (this->m_location < BOARD ? HAND_COLS : BOARD_COLS); }

	// Member variables
	// ----------------
	int m_ID;

	Location m_location;
	Color m_color;
};

#endif // GAME_SQUARE_H
