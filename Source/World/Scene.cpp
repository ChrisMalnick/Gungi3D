/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Scene.cpp
 * 
 * Summary:	Manages all internal and imported models as well as handles
 *		animations and mouse cursor interactions
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

#include "World/Scene.h"

#include <glm/gtc/matrix_transform.hpp>

// Member functions
// ----------------
void Scene::init()
{
	this->m_light.init();
	this->m_picture_model.init();
}

void Scene::build()
{
	int settings[2] = { 0, 0 };
	
	this->m_state.build();
	this->m_state.init(settings);
	
	// Models
	this->m_board_model.build("./Resources/Models/Board/Board.obj");
	this->m_stand_model.build("./Resources/Models/Stand/Stand.obj");

	this->m_tatami_model.build("./Resources/Models/Tatami/Tatami.obj");

	this->m_planks_model.build("./Resources/Models/Planks/Planks.obj");
	this->m_platform_model.build("./Resources/Models/Platform/Platform.obj");

	this->m_panels_model.build("./Resources/Models/Panels/Panels.obj");

	this->m_wall_door_model.build("./Resources/Models/Wall/Wall_Door.obj");
	this->m_wall_window_model.build("./Resources/Models/Wall/Wall_Window.obj");

	this->m_frame_door_model.build("./Resources/Models/Frame/Frame_Door.obj");
	this->m_frame_window_model.build("./Resources/Models/Frame/Frame_Window.obj");

	this->m_shoji_door_model.build("./Resources/Models/Shoji/Shoji_Door.obj");
	this->m_shoji_window_model.build("./Resources/Models/Shoji/Shoji_Window.obj");
	this->m_shoji_lantern_model.build("./Resources/Models/Shoji/Shoji_Lantern.obj");

	this->m_scroll_model.build("./Resources/Models/Scroll/Scroll.obj");

	// Picture is single model with set of swapable textures
	this->m_picture_model.build("./Resources/Models/Picture/Picture.obj");
	this->m_picture_model.loadTextures();
	this->m_picture_model.init();

	// Piece is single model with set of swapable textures
	this->m_piece_model.build("./Resources/Models/Piece/Piece.obj");
	this->m_piece_model.loadTextures();

	// Square is textureless quad that utilizes RGBA colorspace
	this->m_square_model.build();
}

// Updates queued animations and determines if they all have complete
bool Scene::update(double p_delta_time)
{
	std::vector<Animation> *animations_ptr = this->m_state.getBoardPtr()->getAnimationsPtr();
	
	for (auto i = animations_ptr->begin(); i != animations_ptr->end();)
	{
		if ((*animations_ptr)[i - animations_ptr->begin()].update(p_delta_time))
			i = animations_ptr->erase(i);

		else
			++i;
	}

	return animations_ptr->empty();
}

void Scene::renderModels(const Shader &p_shader_ref)
{
	glm::mat4 model;

	p_shader_ref.setFloat("u_alpha", 1.0f);
	p_shader_ref.setBool("u_depthmap.sample_cube", true);

	p_shader_ref.setFloat("u_material.shininess", 2.0f);
	p_shader_ref.setBool("u_depthmap.sample_2D", true);

	// Tatami models
	// -------------
	// Left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-2.0f, -1.05f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_tatami_model.render(p_shader_ref);

	// Right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.0f, -1.05f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_tatami_model.render(p_shader_ref);

	p_shader_ref.setFloat("u_material.shininess", 64.0f);
	p_shader_ref.setBool("u_depthmap.sample_2D", false);

	// Planks models
	// -------------
	// Left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-5.0f, -1.05f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_planks_model.render(p_shader_ref);

	// Right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.0f, -1.05f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_planks_model.render(p_shader_ref);

	// Far right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(7.0f, -1.05f, 0.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_planks_model.render(p_shader_ref);

	// Platform models
	// ---------------
	// Back
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -0.95f, -5.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_platform_model.render(p_shader_ref);

	// Front
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -0.95f, 5.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_platform_model.render(p_shader_ref);

	p_shader_ref.setFloat("u_material.shininess", 2.0f);

	// Panels models
	// -------------
	// Back
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 2.05f, -6.2f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_panels_model.render(p_shader_ref);

	// Front
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 2.05f, 6.2f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_panels_model.render(p_shader_ref);

	// Wall models
	// -----------
	// Window
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-6.2f, 2.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_wall_window_model.render(p_shader_ref);

	// Door
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.2f, 2.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_wall_door_model.render(p_shader_ref);

	p_shader_ref.setFloat("u_material.shininess", 64.0f);
	
	// Frame models
	// ------------
	// Window
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-6.2f, 2.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_frame_window_model.render(p_shader_ref);

	// Door
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.2f, 1.8f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_frame_door_model.render(p_shader_ref);

	p_shader_ref.setFloat("u_material.shininess", 2.0f);
	
	// Shoji models
	// ------------
	// Window
	// Left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-6.225f, 2.0f, 2.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_window_model.render(p_shader_ref);

	// Middle
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-6.175f, 2.0f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_window_model.render(p_shader_ref);

	// Right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-6.225f, 2.0f, -2.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_window_model.render(p_shader_ref);
	
	// Door
	// Left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.225f, 1.8f, -2.5f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_door_model.render(p_shader_ref);

	// Middle
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.175f, 1.8f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_door_model.render(p_shader_ref);

	// Right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(6.225f, 1.8f, 2.5f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_door_model.render(p_shader_ref);

	// Lantern
	// Back left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-5.0f, -0.3625f, -5.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_lantern_model.render(p_shader_ref);

	// Back right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.0f, -0.3625f, -5.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_lantern_model.render(p_shader_ref);

	// Front left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-5.0f, -0.3625f, 5.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_lantern_model.render(p_shader_ref);

	// Front right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.0f, -0.3625f, 5.0f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_shoji_lantern_model.render(p_shader_ref);
	
	// Picture models
	// --------------
	// Back left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-2.5f, 2.0f, -5.9875f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_picture_model.render(p_shader_ref, 0);

	// Back right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.5f, 2.0f, -5.9875f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_picture_model.render(p_shader_ref, 1);

	// Front left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-2.5f, 2.0f, 5.9875f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_picture_model.render(p_shader_ref, 2);

	// Front right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(2.5f, 2.0f, 5.9875f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_picture_model.render(p_shader_ref, 3);
	
	// Scroll models
	// -------------
	// Back left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.9875f, 2.0f, -5.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_scroll_model.render(p_shader_ref);

	// Front left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.9875f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_scroll_model.render(p_shader_ref);

	// Back right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-5.9875f, 2.0f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_scroll_model.render(p_shader_ref);

	// Front right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-5.9875f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_scroll_model.render(p_shader_ref);
	
	p_shader_ref.setFloat("u_material.shininess", 32.0f);
	p_shader_ref.setBool("u_depthmap.sample_2D", true);
	
	// Board model
	// -----------
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_board_model.render(p_shader_ref);

	// Stand models
	// ------------
	// Left
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(1.35f, -0.55f, 0.3f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_stand_model.render(p_shader_ref);

	// Right
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(-1.35f, -0.55f, -0.3f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_stand_model.render(p_shader_ref);
}

void Scene::renderPieces(const Shader &p_shader_ref)
{
	p_shader_ref.setFloat("u_material.shininess", 64.0f);

	p_shader_ref.setBool("u_depthmap.sample_cube", false);
	p_shader_ref.setBool("u_depthmap.sample_2D", true);

	this->renderBoardPieces(p_shader_ref);

	this->renderHandPieces(p_shader_ref, game::Piece::BLACK);
	this->renderHandPieces(p_shader_ref, game::Piece::WHITE);
}

// Squares use separate shader from models
void Scene::renderSquares(const Shader &p_shader_ref)
{
	p_shader_ref.setFloat("u_overlay.brightness", this->m_light.getBrightness());

	Set *set_ptr = this->m_state.getSetPtr();

	for (int i = 0; i < NUM_SQUARES; ++i)
		this->renderSquare(p_shader_ref, set_ptr->getSquarePtr(i));
}

// Determine which square (if any) mouse cursor is over
// Note: board squares are physically higher than hand squares
void Scene::mousePick(glm::vec3 p_position, glm::vec3 p_direction)
{
	this->clearSquares();

	Set *set_ptr = this->m_state.getSetPtr();

	for (int i = game::Square::BOARD; i < NUM_SQUARES; ++i)
	{
		if (this->pickSquare(p_position, p_direction, set_ptr->getSquarePtr(i)))
			return;
	}

	for (int i = 0; i < game::Square::BOARD; ++i)
	{
		if (this->pickSquare(p_position, p_direction, set_ptr->getSquarePtr(i)))
			return;
	}
}

void Scene::renderBoardPieces(const Shader &p_shader_ref)
{
	Board *board_ptr = this->m_state.getBoardPtr();

	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			int height = 0;
			
			for (auto &elem : board_ptr->getStackRef(j, i))
			{
				glm::mat4 model;

				++height;

				if (!this->handleBoardAnimation(model, elem))
				{
					float y = 0.0f;

					for (int k = 1; k < height; ++k)
						y += (board_ptr->getPiecePtr(j, i, k)->getSide() != board_ptr->getPiecePtr(j, i, k - 1)->getSide() ? 0.02f : 0.015f);

					model = glm::translate(model, glm::vec3(0.0f + 0.2f * (static_cast<float>(j) - 4.0f), 0.01f + y, 0.0f + 0.2f * (static_cast<float>(i) - 4.0f)));
				}

				p_shader_ref.setFloat("u_alpha", 1.0f);

				this->renderPiece(p_shader_ref, model, elem);
			}
		}
	}
}

void Scene::renderHandPieces(const Shader &p_shader_ref, game::Piece::Color p_color)
{
	Hand *hand_ptr = this->m_state.getHandPtr(p_color);
	int sign = (p_color == game::Piece::WHITE ? -1 : 1);
	
	for (int i = 0; i < HAND_ROWS; ++i)
	{
		for (int j = 0; j < HAND_COLS; ++j)
		{
			int height = 0;
			
			for (auto &elem : hand_ptr->getStackRef(j, i))
			{
				glm::mat4 model;

				float y = 0.015f * height++;

				if (!this->handleHandAnimation(p_shader_ref, model, elem))
				{
					model = glm::translate(model, glm::vec3(sign * (1.85f + 0.2f * (static_cast<float>(j) - 4.0f)), -0.09f + y, sign * (0.6f + 0.2f * (static_cast<float>(i) - 4.0f))));
					p_shader_ref.setFloat("u_alpha", 1.0f);
				}

				this->renderPiece(p_shader_ref, model, elem);
			}
		}
	}
}

void Scene::renderPiece(const Shader &p_shader_ref, glm::mat4 &p_model_ref, game::Piece *p_piece_ptr)
{
	if (p_piece_ptr->getAlignment() == game::Piece::WHITE)
		p_model_ref = glm::rotate(p_model_ref, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	if (p_piece_ptr->getSide() == game::Piece::BACK)
		p_model_ref = glm::rotate(p_model_ref, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	p_model_ref = glm::scale(p_model_ref, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", p_model_ref);

	this->m_piece_model.render(p_shader_ref, p_piece_ptr);
}

void Scene::renderSquare(const Shader &p_shader_ref, game::Square *p_square_ptr)
{
	if (p_square_ptr->getColor() == game::Square::CLEAR)
		return;

	glm::mat4 model;

	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	switch (p_square_ptr->getLocation())
	{
	case game::Square::BLACK_HAND:
		model = glm::translate(model, glm::vec3(1.85f + 0.2f * (static_cast<float>(x) - 4.0f), -0.0999f, 0.6f + 0.2f * (static_cast<float>(y) - 4.0f)));
		break;

	case game::Square::WHITE_HAND:
		model = glm::translate(model, glm::vec3(-1.85f - 0.2f * (static_cast<float>(x) - 4.0f), -0.0999f, -0.6f - 0.2f * (static_cast<float>(y) - 4.0f)));
		break;

	case game::Square::BOARD:
		model = glm::translate(model, glm::vec3(0.2f * (static_cast<float>(x) - 4.0f), 0.0001f, 0.2f * (static_cast<float>(y) - 4.0f)));
		break;
	}

	model = glm::scale(model, glm::vec3(0.1f));

	p_shader_ref.setMat4("u_model", model);

	this->m_square_model.render(p_shader_ref, p_square_ptr->getRGBA());
}

void Scene::clearSquares()
{
	this->m_state.getBoardPtr()->clear(game::Square::PURPLE);

	if (this->m_state.getCurrSquarePtr() == nullptr)
		return;

	if (this->m_state.getCurrPiecePtr() == nullptr)
		this->m_state.getCurrSquarePtr()->setColor(game::Square::CLEAR);

	else if (this->m_state.getCurrSquarePtr()->getColor() == game::Square::GRAY)
		this->m_state.getCurrSquarePtr()->setColor(game::Square::CLEAR);

	this->m_state.setCurrSquarePtr(nullptr);
}

bool Scene::handleBoardAnimation(glm::mat4 &p_model_ref, game::Piece *p_piece_ptr)
{
	for (auto &elem : *this->m_state.getBoardPtr()->getAnimationsPtr())
	{
		if (!elem.inBoard())
			continue;

		if (elem.getPiecePtr() != p_piece_ptr)
			continue;

		p_model_ref = glm::translate(p_model_ref, elem.getPos());

		if (elem.move() && !elem.vertical())
			p_model_ref = glm::rotate(p_model_ref, elem.getAngle(), elem.getAxis());

		if (elem.flip())
			p_model_ref = glm::rotate(p_model_ref, elem.sign() * glm::radians(elem.getAngle()), elem.getAxis());

		return true;
	}

	return false;
}

bool Scene::handleHandAnimation(const Shader &p_shader_ref, glm::mat4 &p_model_ref, game::Piece *p_piece_ptr)
{
	for (auto &elem : *this->m_state.getBoardPtr()->getAnimationsPtr())
	{
		if (elem.inBoard())
			continue;

		if (elem.getPiecePtr() != p_piece_ptr)
			continue;
		
		p_model_ref = glm::translate(p_model_ref, elem.getPos());

		if (elem.move() && !elem.vertical())
			p_model_ref = glm::rotate(p_model_ref, elem.getAngle(), elem.getAxis());

		if (elem.fadeOut() && !elem.move() && !elem.skipRotation())
		{
			p_model_ref = glm::rotate(p_model_ref, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			p_model_ref = glm::rotate(p_model_ref, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		p_shader_ref.setFloat("u_alpha", elem.getAlpha());

		return true;
	}

	return false;
}

bool Scene::pickSquare(glm::vec3 p_position, glm::vec3 p_direction, game::Square *p_square_ptr)
{
	game::Square::Location location = p_square_ptr->getLocation();
	
	glm::vec3 intersect = p_position - p_direction * ((glm::dot(p_position, SQUARE_NORMAL) + (location < game::Square::BOARD ? 0.1f : 0.0f)) / glm::dot(p_direction, SQUARE_NORMAL));

	int x = p_square_ptr->getX();
	int y = p_square_ptr->getY();

	glm::vec3 offset;

	switch (location)
	{
	case game::Square::BLACK_HAND:
		offset = glm::vec3(1.85f + 0.2f * (static_cast<float>(x) - 4.0f), -0.0999f, 0.6f + 0.2f * (static_cast<float>(y) - 4.0f));
		break;

	case game::Square::WHITE_HAND:
		offset = glm::vec3(-1.85f - 0.2f * (static_cast<float>(x) - 4.0f), -0.0999f, -0.6f - 0.2f * (static_cast<float>(y) - 4.0f));
		break;

	case game::Square::BOARD:
		offset = glm::vec3(0.2f * (static_cast<float>(x) - 4.0f), 0.0001f, 0.2f * (static_cast<float>(y) - 4.0f));
		break;
	}

	if (intersect.x >= offset.x - 0.0975f && intersect.x <= offset.x + 0.0975f && intersect.z >= offset.z - 0.0975f && intersect.z <= offset.z + 0.0975f)
	{
		this->m_state.setCurrSquarePtr(p_square_ptr);
		
		switch (location)
		{
		case game::Square::BLACK_HAND:
			this->m_state.getBlackHandPtr()->mouseOver();
			break;

		case game::Square::WHITE_HAND:
			this->m_state.getWhiteHandPtr()->mouseOver();
			break;

		case game::Square::BOARD:
			this->m_state.getBoardPtr()->mouseOver();
			break;
		}

		if (!this->m_state.getActivePlayerPtr()->controllable() || this->m_state.gameOver())
			this->m_state.getCurrSquarePtr()->setColor(game::Square::GRAY);

		return true;
	}

	return false;
}
