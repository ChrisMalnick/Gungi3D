/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Animation.cpp
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

#include "World/Animation.h"

float Animation::m_speed_mod = SPEED_NORMAL;

 // Class functions
 // ---------------
 // Constructor
Animation::Animation(game::Piece *p_piece_ptr, glm::vec3 p_src, glm::vec3 p_end, glm::vec3 p_mid)
{
	this->m_piece_ptr = p_piece_ptr;

	this->m_pos = this->m_src = p_src;
	this->m_end = p_end;
	this->m_mid = p_mid;

	// Calculate required arc and tilt for lateral moves
	if (this->move() && !this->vertical())
	{
		glm::vec3 dest = this->dest();
		
		float slope = (dest.z - p_src.z) / (dest.x - p_src.x);
		float angle = glm::atan(slope);

		p_src.x > dest.x ? angle += glm::pi<float>() / 2 : angle -= glm::pi<float>() / 2;

		this->m_axis.x = glm::cos(angle);
		this->m_axis.z = glm::sin(angle);

		this->m_angle = 0.0f;
	}
}

// Member functions
// ----------------
// Updates animation compositely and determines if it has completed
bool Animation::update(double p_delta_time)
{
	bool result = true;
	
	if (this->move())
	{
		if (this->vertical())
			result = this->verticalMove(p_delta_time) && result;

		else
			result = this->lateralMove(p_delta_time) && result;
	}

	if (!this->inBoard())
		result = this->fade(p_delta_time) && result;

	if (this->flip())
		result = this->flip(p_delta_time);

	return result;
}

// Updates lateral movement and determines if it has completed
bool Animation::lateralMove(double p_delta_time)
{
	glm::vec3 dest = this->dest();
	
	glm::vec3 delta = dest - this->m_pos;
	glm::vec3 rate = delta * static_cast<float>(p_delta_time) * m_speed_mod;

	if (this->m_pos.x != dest.x)
	{
		if (glm::abs(delta.x) < 0.0001f)
			this->m_pos.x = dest.x;

		else
		{
			this->m_pos.x += rate.x;

			if (this->outOfRange(this->m_pos.x, this->m_src.x, dest.x))
				this->m_pos.x = dest.x;
		}
	}

	if (this->m_pos.z != dest.z)
	{
		if (glm::abs(delta.z) < 0.0001f)
			this->m_pos.z = dest.z;

		else
		{
			this->m_pos.z += rate.z;

			if (this->outOfRange(this->m_pos.z, this->m_src.z, dest.z))
				this->m_pos.z = dest.z;
		}
	}

	float dist_total = this->dist(this->m_src, dest);
	float dist_remain = this->dist(this->m_pos, dest);

	float ysrc = this->m_src.y * dist_remain / dist_total;
	float ydest = dest.y * (dist_total - dist_remain) / dist_total;

	this->m_pos.y = ysrc + ydest + glm::sin(dist_remain / dist_total * glm::pi<float>()) * dist_total * 0.1f;
	this->m_angle = glm::sin(dist_remain / dist_total * glm::pi<float>()) * dist_total * 0.25f;

	return (this->m_pos.x == dest.x && this->m_pos.z == dest.z);
}

// Updates vertical movement and determines if it has completed
bool Animation::verticalMove(double p_delta_time)
{
	float ydest = this->dest().y;
	
	float ydelta = ydest - this->m_pos.y;
	float yrate = ydelta * static_cast<float>(p_delta_time) * m_speed_mod;
	
	if (this->m_pos.y != ydest)
	{
		if (glm::abs(ydelta) < 0.0001f)
			this->m_pos.y = ydest;

		else
		{
			this->m_pos.y += yrate;

			if (this->outOfRange(this->m_pos.y, this->m_src.y, ydest))
				this->m_pos.y = ydest;
		}
	}
	
	return (this->m_pos.y == ydest);
}

// Updates fade and determines if it has completed
bool Animation::fade(double p_delta_time)
{
	float rate = static_cast<float>(p_delta_time) * m_speed_mod;
	
	if (this->m_pos == this->m_end)
	{
		if ((this->m_alpha += rate) >= 1.0f)
		{
			this->m_alpha = 1.0f;
			return true;
		}
	}

	else
	{
		if ((this->m_alpha -= rate) <= 0.0f)
		{
			this->m_alpha = 0.0f;

			this->m_pos = this->m_end;
			this->m_mid = glm::vec3(0.0f);
		}
	}

	return false;
}

// Updates rotation and determines if it has completed
bool Animation::flip(double p_delta_time)
{
	float rate = this->m_angle * static_cast<float>(p_delta_time) * m_speed_mod;
	
	if (this->m_angle != 0.0f)
	{
		if (this->m_angle < 0.1f)
			this->m_angle = 0.0f;

		else
			this->m_angle -= rate;
	}
	
	return (this->m_angle == 0.0f);
}
