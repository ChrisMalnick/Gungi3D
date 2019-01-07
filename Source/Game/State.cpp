/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	State.cpp
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

#include "Game/State.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Member functions
// ----------------
void State::init(int *p_settings_ptr)
{
	this->m_turn = 1;
	this->m_stalemate = false;
	
	this->m_curr_piece_ptr = nullptr;
	this->m_curr_square_ptr = nullptr;

	this->m_set.init();

	this->m_black_hand.removeAll();
	this->m_white_hand.removeAll();

	this->m_board.init();
	this->m_positions.clear();

	if (p_settings_ptr != nullptr)
	{
		this->m_black_hand.init(this->m_set);
		this->m_white_hand.init(this->m_set);

		this->m_black_player.init(p_settings_ptr[0]);
		this->m_white_player.init(p_settings_ptr[1]);
	}
}

void State::build()
{
	this->m_black_hand.build(this->m_set);
	this->m_white_hand.build(this->m_set);

	this->m_board.build(this->m_set);
}

void State::mouseClick(int p_button)
{
	// No interaction takes place if active player is not controlled
	if (!this->getActivePlayerPtr()->controllable())
		return;

	// No interaction takes place if no square is selected
	if (this->m_curr_square_ptr == nullptr)
		return;
	
	// No interaction takes place if selected square cannot be interacted with
	if (this->m_curr_square_ptr->getColor() == game::Square::GRAY)
		return;

	int turn = this->m_turn;

	switch (this->m_curr_square_ptr->getLocation())
	{
	case game::Square::BLACK_HAND:
		if (p_button == MOUSE_BUTTON_1)
		{
			this->m_black_hand.mouseButton1();
			this->handleHandMB1();
		}

		break;

	case game::Square::WHITE_HAND:
		if (p_button == MOUSE_BUTTON_1)
		{
			this->m_white_hand.mouseButton1();
			this->handleHandMB1();
		}

		break;

	case game::Square::BOARD:
		switch (p_button)
		{
		case MOUSE_BUTTON_1:
			this->m_board.mouseButton1();
			break;

		case MOUSE_BUTTON_2:
			this->m_board.mouseButton2();
			break;

		case MOUSE_BUTTON_4:
			this->m_board.mouseButton4();
			break;

		case MOUSE_BUTTON_5:
			this->m_board.mouseButton5();
			break;
		}

		break;
	}

	// Action occurred
	if (this->m_turn != turn)
	{
		if (!this->getActivePlayerPtr()->controllable() || this->gameOver())
			this->m_curr_square_ptr->setColor(game::Square::GRAY);
		
		// Only record positions after initial arrangement
		if (this->m_turn > INITIAL_ARRANGEMENT)
			this->updatePositions();
	}
}

void State::handleHandMB1()
{
	if (this->m_curr_piece_ptr == nullptr)
		this->m_board.clear(game::Square::BLUE);

	else
		this->m_turn > INITIAL_ARRANGEMENT ? this->m_board.setDroppable() : this->m_board.setPlaceable();
}

bool State::handleAI()
{
	game::Piece::Color active = this->getActiveColor(this->m_turn);
	
	if (this->getPlayerPtr(active)->controllable())
		return false;

	if (this->gameOver())
		return false;

	int turn = this->m_turn;

	if (this->getPlayerPtr(active)->act())
	{
		// Action did not occur
		if (this->m_turn == turn)
			this->m_board.getCheckmateRef(active) = true;

		// Only record positions after initial arrangement
		else if (this->m_turn > INITIAL_ARRANGEMENT)
			this->updatePositions();

		return true;
	}

	return false;
}

// Record current game representation to file
void State::save(const std::string &p_path_ref)
{
	std::ofstream file;
	file.open(p_path_ref, std::ios_base::out);

	if (!file.is_open())
	{
		std::cerr << "ERROR::SCENE::SAVE::FILE::OPEN_FAILED >> " << p_path_ref << std::endl;
		return;
	}

	if (!file.good())
	{
		std::cerr << "ERROR::SCENE::SAVE::FILE::BAD_STREAM >> " << p_path_ref << std::endl;
		std::cerr << "eofbit::" << file.eof() << " | " << "failbit::" << file.fail() << " | " << "badbit::" << file.bad() << std::endl;

		return;
	}

	std::ostringstream oss;
	std::string line;

	// First line is for various parameters
	oss << this->m_turn << ';' << this->m_black_player.getLevel() << ';' << this->m_white_player.getLevel() << ';';

	// Encrypt line and wite it to file
	line = this->XOR(oss.str());
	file << line << std::endl;
	oss.str("");

	// Second line is for game configuration
	for (int i = 0; i < BOARD_ROWS; ++i)
	{
		for (int j = 0; j < BOARD_COLS; ++j)
		{
			// Board
			oss << this->m_board.getSquarePtr(j, i)->getID() << ':';

			for (auto &elem : this->m_board.getStackRef(j, i))
				oss << elem->getID() << "-" << elem->getSide() << ',';

			oss << ';';

			if (i < HAND_ROWS && j < HAND_COLS)
			{
				// Black hand
				oss << this->m_black_hand.getSquarePtr(j, i)->getID() << ':';

				for (auto &elem : this->m_black_hand.getStackRef(j, i))
					oss << elem->getID() << "-" << elem->getSide() << ',';

				oss << ';';

				// White hand
				oss << this->m_white_hand.getSquarePtr(j, i)->getID() << ':';

				for (auto &elem : this->m_white_hand.getStackRef(j, i))
					oss << elem->getID() << "-" << elem->getSide() << ',';

				oss << ';';
			}
		}
	}

	// Encrypt line and wite it to file
	line = this->XOR(oss.str());
	file << line << std::endl;
	oss.str("");

	// Third line if for exchanges
	for (auto &elem : this->m_board.getExchangesRef())
		oss << elem.square_ptr->getID() << ":" << elem.turn << ";";

	// Encrypt line and wite it to file
	line = this->XOR(oss.str());
	file << line << std::endl;
	oss.str("");

	// Remaining lines are for positions
	for (auto &elem1 : this->m_positions)
	{
		oss << elem1.getCount() << ':';

		for (int i = 0; i < BOARD_ROWS; ++i)
		{
			for (int j = 0; j < BOARD_COLS; ++j)
			{
				for (auto &elem2 : elem1.getStackRef(j, i))
					oss << elem2 << ',';

				oss << ';';
			}
		}

		// Encrypt line and wite it to file
		line = this->XOR(oss.str());
		file << line << std::endl;
		oss.str("");
	}

	file.close();
}

// Restore previously recorded game from file
void State::load(const std::string &p_path_ref)
{
	std::ifstream file;
	file.open(p_path_ref, std::ios_base::in);

	if (!file.is_open())
	{
		std::cerr << "ERROR::SCENE::LOAD::FILE::OPEN_FAILED >> " << p_path_ref << std::endl;
		return;
	}

	if (!file.good())
	{
		std::cerr << "ERROR::SCENE::LOAD::FILE::BAD_STREAM >> " << p_path_ref << std::endl;
		std::cerr << "eofbit::" << file.eof() << " | " << "failbit::" << file.fail() << " | " << "badbit::" << file.bad() << std::endl;

		return;
	}

	this->init();

	std::string line;
	int count1 = 0;

	// Parse through file line by line
	while (std::getline(file, line))
	{
		// Decrypt line
		line = this->XOR(line);

		// First line contains various parameters
		if (count1 == 0)
		{
			std::istringstream iss1(line);
			std::string token1;

			std::getline(iss1, token1, ';');
			this->m_turn = std::stoi(token1);

			std::getline(iss1, token1, ';');
			this->m_black_player.init(std::stoi(token1));

			std::getline(iss1, token1, ';');
			this->m_white_player.init(std::stoi(token1));
		}

		// Second line contains game configuration
		else if (count1 == 1)
		{
			std::istringstream iss1(line);
			std::string token1;

			while (std::getline(iss1, token1, ';'))
			{
				std::size_t pos = token1.find(':');

				game::Square *square_ptr = this->m_set.getSquarePtr(std::stoi(token1.substr(0, pos)));

				std::istringstream iss2(token1.substr(pos + 1));
				std::string token2;

				while (std::getline(iss2, token2, ','))
				{
					pos = token2.find('-');

					game::Piece *piece_ptr = this->m_set.getPiecePtr(std::stoi(token2.substr(0, pos)));

					if (std::stoi(token2.substr(pos + 1)) == game::Piece::BACK)
						piece_ptr->flip();

					switch (square_ptr->getLocation())
					{
					case game::Square::BLACK_HAND:
						this->m_black_hand.getStackRef(square_ptr).push_back(piece_ptr);

						if (this->m_turn > INITIAL_ARRANGEMENT && piece_ptr->impartsMRE())
							this->m_curr_piece_ptr = piece_ptr;

						break;

					case game::Square::WHITE_HAND:
						this->m_white_hand.getStackRef(square_ptr).push_back(piece_ptr);

						if (this->m_turn > INITIAL_ARRANGEMENT && piece_ptr->impartsMRE())
							this->m_curr_piece_ptr = piece_ptr;

						break;

					case game::Square::BOARD:
						this->m_board.setPiecePtr(piece_ptr, square_ptr);
						break;
					}
				}
			}
		}

		// Third line contains exchanges
		else if (count1 == 2)
		{
			std::istringstream iss1(line);
			std::string token1;

			while (std::getline(iss1, token1, ';'))
			{
				std::size_t pos = token1.find(':');
				this->m_board.getExchangesRef().push_back({ this->m_set.getSquarePtr(std::stoi(token1.substr(0, pos))), std::stoi(token1.substr(pos + 1)) });
			}
		}

		// Remaining lines contain positions
		else
		{
			Position position;
			
			std::size_t pos = line.find(':');

			position.setCount(std::stoi(line.substr(0, pos)));
			
			std::istringstream iss1(line.substr(pos + 1));
			std::string token1;

			int count2 = 0;

			while (std::getline(iss1, token1, ';'))
			{
				std::istringstream iss2(token1);
				std::string token2;

				while (std::getline(iss2, token2, ','))
					position.setID(std::stoi(token2), count2);

				++count2;
			}

			this->m_positions.push_back(position);
		}

		++count1;
	}

	for (auto &elem : this->m_positions)
	{
		if (elem.getCount() >= STALEMATE)
		{
			this->m_stalemate = true;
			break;
		}
	}

	if (this->m_turn > INITIAL_ARRANGEMENT)
	{
		this->m_board.getBlackCheckRef() = this->m_board.check(game::Piece::BLACK);
		this->m_board.getWhiteCheckRef() = this->m_board.check(game::Piece::WHITE);

		game::Piece::Color active = this->getActiveColor(this->m_turn);
		game::Piece::Color passive = this->getPassiveColor(this->m_turn);

		if (!(this->m_board.getCheckmateRef(passive) = this->m_board.getCheckRef(passive)))
			this->m_board.getCheckmateRef(active) = this->m_board.checkmate(this->m_turn, this->getHandPtr(active), this->getHandPtr(passive));
	}

	// Saved during forced rearrangement
	if (this->m_curr_piece_ptr != nullptr)
		this->m_board.setRearrangeable();

	file.close();
}

void State::updatePositions()
{
	Position position(this->m_board);

	for (auto &elem : this->m_positions)
	{
		if (elem.equals(position))
		{
			elem.increment();
			this->m_stalemate = (elem.getCount() >= STALEMATE);

			return;
		}
	}

	this->m_positions.push_back(position);
}

// For encryption and decryption of game files
std::string State::XOR(const std::string &p_line_ref)
{
	std::string output = p_line_ref;

	char keys[4] = { 'K', 'E', 'Y', 'S', };

	for (unsigned int i = 0; i < p_line_ref.size(); ++i)
		output[i] = p_line_ref[i] ^ keys[i % (sizeof(keys) / sizeof(char))];

	return output;
}
