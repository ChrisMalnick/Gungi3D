/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Scene.h
 *
 * Summary:	Manages all internal and imported models as well as handles
 *			animations and mouse cursor interactions
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

#ifndef SCENE_H
#define SCENE_H

#include "Game/State.h"
#include "World/Light.h"
#include "World/Square.h"
#include "World/Picture.h"
#include "World/Piece.h"

static const glm::vec3 SQUARE_NORMAL = glm::vec3(0.0f, 1.0f, 0.0f);

class Scene
{
public:
	// Member functions
	// ----------------
	inline State* getStatePtr() { return &this->m_state; }
	inline Light* getLightPtr() { return &this->m_light; }

	void init();
	void build();

	bool update(double p_delta_time); // Updates queued animations and determines if they all have complete

	void renderModels(const Shader &p_shader_ref);
	void renderPieces(const Shader &p_shader_ref);
	void renderSquares(const Shader &p_shader_ref); // Squares use separate shader from models

	void mousePick(glm::vec3 p_position, glm::vec3 p_direction); // Determine which square (if any) mouse cursor is over

private:
	void renderBoardPieces(const Shader &p_shader_ref);
	void renderHandPieces(const Shader &p_shader_ref, game::Piece::Color p_color);

	void renderPiece(const Shader &p_shader_ref, glm::mat4 &p_model_ref, game::Piece *p_piece_ptr);
	void renderSquare(const Shader &p_shader_ref, game::Square *p_square_ptr);

	void clearSquares();

	bool handleBoardAnimation(glm::mat4 &p_model_ref, game::Piece *p_piece_ptr);
	bool handleHandAnimation(const Shader &p_shader_ref, glm::mat4 &p_model_ref, game::Piece *p_piece_ptr);

	bool pickSquare(glm::vec3 p_position, glm::vec3 p_direction, game::Square *p_square_ptr);

	// Member variables
	// ----------------
	State m_state; // Current representation of game

	Light m_light;

	// Models
	Model m_board_model;
	Model m_stand_model;

	Model m_tatami_model;

	Model m_planks_model;
	Model m_platform_model;

	Model m_panels_model;

	Model m_wall_door_model;
	Model m_wall_window_model;

	Model m_frame_door_model;
	Model m_frame_window_model;

	Model m_shoji_door_model;
	Model m_shoji_window_model;
	Model m_shoji_lantern_model;

	Model m_scroll_model;

	Picture m_picture_model; // Special model for pictures

	gfx::Piece m_piece_model; // Special model for pieces
	gfx::Square m_square_model; // Special model for squares
};

#endif // SCENE_H