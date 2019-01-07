/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Animation.h
 * 
 * Summary:	Manages the various properties that determine the position,
 *		orientation, and opacity of a given piece upon interaction relative
 *		to the scene
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

#include <glm/gtc/matrix_transform.hpp>

#ifndef ANIMATION_H
#define ANIMATION_H

static const glm::vec3 SKIP_ROTATION = glm::vec3(-1.0f);

static const float SPEED_NORMAL = 1.0f;
static const float SPEED_FAST = 4.0f;

class Animation
{
public:
	// Class functions
	// ---------------
	// Constructor
	Animation(game::Piece *p_piece_ptr, glm::vec3 p_src = glm::vec3(0.0f), glm::vec3 p_end = glm::vec3(0.0f), glm::vec3 p_mid = glm::vec3(0.0f));

	// Member functions
	// ----------------
	inline game::Piece* getPiecePtr() { return this->m_piece_ptr; }

	inline glm::vec3 getPos() { return this->m_pos; }

	inline glm::vec3 getAxis() { return this->m_axis; }
	inline float getAngle() { return this->m_angle; }

	inline float getAlpha() { return this->m_alpha; }

	static inline friend void setSpeedMod(float p_speed_mod);

	inline glm::vec3 dest() { return (this->m_mid == glm::vec3(0.0f) ? this->m_end : this->m_mid); }

	inline float dist(glm::vec3 p_src, glm::vec3 p_dest) { return glm::sqrt(glm::pow(p_dest.x - p_src.x, 2) + glm::pow(p_dest.z - p_src.z, 2)); }

	inline float sign() { return (this->m_piece_ptr->getAlignment() == game::Piece::WHITE ? 1.0f : -1.0f); }

	inline bool vertical() { return this->aligned(this->m_src, this->dest()); }
	inline bool aligned(glm::vec3 p_src, glm::vec3 p_dest) { return (p_src.x == p_dest.x && p_src.z == p_dest.z); }

	inline bool fadeOut() { return (this->m_pos != this->m_end); }
	inline bool inBoard() { return (glm::abs(this->m_end.x) <= 0.8f); }

	inline bool move() { return (this->m_mid != glm::vec3(0.0f) && this->m_mid != SKIP_ROTATION || this->inBoard() && !this->flip()); }
	inline bool flip() { return (this->m_end == glm::vec3(0.0f) && this->m_mid == glm::vec3(0.0f)); }

	inline bool skipRotation() { return (this->m_mid == SKIP_ROTATION); }

	inline bool outOfRange(float p_pos, float p_src, float p_dest) { return (p_dest > p_src && p_pos > p_dest || p_dest < p_src && p_pos < p_dest); }

	bool update(double p_delta_time); // Updates animation compositely and determines if it has completed

	bool lateralMove(double p_delta_time); // Updates lateral movement and determines if it has completed
	bool verticalMove(double p_delta_time); // Updates vertical movement and determines if it has completed

	bool fade(double p_delta_time); // Updates fade and determines if it has completed

	bool flip(double p_delta_time); // Updates rotation and determines if it has completed

private:
	// Member variables
	// ----------------
	game::Piece *m_piece_ptr;

	// Position
	glm::vec3 m_pos;

	glm::vec3 m_src;
	glm::vec3 m_end;
	glm::vec3 m_mid;

	// Rotation
	glm::vec3 m_axis = glm::vec3(0.0f, 0.0f, 1.0f);
	float m_angle = 180.0f;

	// Opacity
	float m_alpha = 1.0f;

	// Speed
	static float m_speed_mod;
};

static inline void setSpeedMod(float p_speed_mod) { Animation::m_speed_mod = p_speed_mod; }

#endif // ANIMATION_H
