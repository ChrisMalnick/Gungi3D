/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Camera.cpp
 * 
 * Summary:	Manages the various properties that determine the position and
 *		orientation of the viewing frustum relative to the scene
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

#include "World/Camera.h"

// Member functions
// ----------------
void Camera::init(Mode p_mode)
{
	this->m_position = glm::vec3(0.0f, 0.0f, 3.0f);

	this->m_front = glm::normalize(WORLD_ORIGIN - m_position);
	this->m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
	this->m_up = glm::cross(m_right, m_front);

	this->m_yaw = -90.0f;

	this->m_delta_pitch = 0.0f;
	this->m_delta_yaw = 0.0f;

	this->m_zoom = 45.0f;
	this->m_depth = 3.0f;

	this->m_rotating = false;

	// Set pitch non-zero and update camera's orientation and position
	// Makes camera start at an angle
	if ((this->m_mode = p_mode) == FIXED)
	{
		this->m_pitch = 15.0f;
		this->m_rotatable = true;

		this->rotate(0.0, 0.0);
	}

	else
		this->m_pitch = 0.0f;

	this->m_rotatable = false;
}

// Allows camera to rotate about world's origin based on mouse movements
void Camera::setRotatable(bool p_rotatable)
{
	if (this->m_mode != FIXED)
		return;
	
	if (this->m_rotatable = p_rotatable)
		this->halt();
}

// Sets camera to auto-rotate about world's origin towards specified angles
// Calculates how much camera must rotate about each axis
// Since yaw has 360 degrees of freedom, chooses smaller angle
void Camera::setRotation(float p_pitch, float p_yaw)
{
	if (this->m_mode != FIXED || this->m_rotatable)
		return;
	
	this->m_delta_pitch = p_pitch - this->m_pitch;
	this->m_delta_yaw = p_yaw - this->m_yaw;

	if (glm::abs(this->m_delta_yaw) > 180.0f)
	{
		if (this->m_delta_yaw < 0.0f)
			this->m_delta_yaw += 360.0f;

		else if (this->m_delta_yaw > 0.0f)
			this->m_delta_yaw -= 360.0f;
	}

	this->m_rotating = true;
}

void Camera::adjustPitch(bool p_sign)
{
	if (this->m_mode != FIXED || this->m_rotatable)
		return;

	if (this->m_delta_pitch == 0.0f)
	{
		this->m_delta_pitch = (p_sign ? 0.0f : 15.0f) - std::fmod(this->m_pitch, 15.0f);

		if (this->m_delta_pitch == 0.0f)
			this->m_delta_pitch = -15.0f;
	}

	else
		this->m_delta_pitch += (p_sign ? -15.0f : 15.0f);

	if (this->m_delta_pitch < 0.0f && this->m_pitch + this->m_delta_pitch < 0.0f)
		this->m_delta_pitch = -this->m_pitch;

	if (this->m_delta_pitch > 0.0f && this->m_pitch + this->m_delta_pitch > 89.9f)
		this->m_delta_pitch = 89.9f - this->m_pitch;

	this->m_rotating = true;
}

void Camera::adjustYaw(bool p_sign)
{
	if (this->m_mode != FIXED || this->m_rotatable)
		return;

	if (this->m_delta_yaw == 0.0f)
	{
		this->m_delta_yaw = (p_sign ? 0.0f : 90.0f) - std::fmod(this->m_yaw, 90.0f);

		if (this->m_delta_yaw == 0.0f)
			this->m_delta_yaw = -90.0f;
	}

	else
		this->m_delta_yaw += (p_sign ? -90.0f : 90.0f);

	this->m_rotating = true;
}

// Changes camera's position based on directional input
void Camera::move(Direction p_direction, double p_delta_time)
{
	if (this->m_mode != FREE)
		return;
	
	switch (p_direction)
	{
	case FORWARD:
		this->m_position += this->m_front * static_cast<float>(p_delta_time) * CAMERA_SPEED;
		break;

	case BACKWARD:
		this->m_position -= this->m_front * static_cast<float>(p_delta_time) * CAMERA_SPEED;
		break;

	case LEFT:
		this->m_position -= this->m_right * static_cast<float>(p_delta_time) * CAMERA_SPEED;
		break;

	case RIGHT:
		this->m_position += this->m_right * static_cast<float>(p_delta_time) * CAMERA_SPEED;
		break;
	}
}

// Changes camera's orientation about it's own origin based on mouse movements
void Camera::turn(double p_xoffset, double p_yoffset)
{
	if (this->m_mode != FREE)
		return;
	
	this->m_pitch += static_cast<float>(p_yoffset) * CAMERA_SENSITIVITY;

	if (this->m_pitch < -89.9f)
		this->m_pitch = -89.9f;

	else if (this->m_pitch > 89.9f)
		this->m_pitch = 89.9f;

	this->m_yaw = glm::mod(this->m_yaw + static_cast<float>(p_xoffset) * CAMERA_SENSITIVITY, 360.0f);

	this->orientate();
}

// Changes camera's orientation and position about world's origin based on mouse movements
void Camera::rotate(double p_xoffset, double p_yoffset)
{
	if (this->m_mode != FIXED || !this->m_rotatable)
		return;

	this->m_pitch += static_cast<float>(p_yoffset) * CAMERA_SENSITIVITY;

	if (this->m_pitch < 0.0f)
		this->m_pitch = 0.0f;

	else if (this->m_pitch > 89.9f)
		this->m_pitch = 89.9f;

	this->m_yaw = glm::mod(this->m_yaw + static_cast<float>(p_xoffset) * CAMERA_SENSITIVITY, 360.0f);

	this->orientate();
}

// Advances camera incrementally about world's origin towards its desired orientation and position
// Rate of rotation about an axis decreases as it approaches its desired angle about that axis
// Desired angles for both axes are achieved simultaneously
void Camera::autoRotate(double p_delta_time)
{
	if (this->m_mode != FIXED || !this->m_rotating)
		return;

	float pitch_rate = glm::abs(this->m_delta_pitch * static_cast<float>(p_delta_time));
	float yaw_rate = glm::abs(this->m_delta_yaw * static_cast<float>(p_delta_time));

	if (this->m_delta_pitch < 0.0f)
	{
		this->m_pitch -= pitch_rate;
		this->m_delta_pitch += pitch_rate;
	}

	else if (this->m_delta_pitch > 0.0f)
	{
		this->m_pitch += pitch_rate;
		this->m_delta_pitch -= pitch_rate;
	}

	if (this->m_delta_pitch != 0.0f)
	{
		if (glm::abs(this->m_delta_pitch) < 0.01f)
		{
			this->m_pitch = std::roundf(this->m_pitch + this->m_delta_pitch);
			this->m_delta_pitch = 0.0f;
		}

		if (this->m_pitch < 0.0f)
		{
			this->m_pitch = 0.0f;
			this->m_delta_pitch = 0.0f;
		}

		else if (this->m_pitch > 89.9f)
		{
			this->m_pitch = 89.9f;
			this->m_delta_pitch = 0.0f;
		}
	}

	if (this->m_delta_yaw < 0.0f)
	{
		this->m_yaw -= yaw_rate;
		this->m_delta_yaw += yaw_rate;
	}

	else if (this->m_delta_yaw > 0.0f)
	{
		this->m_yaw += yaw_rate;
		this->m_delta_yaw -= yaw_rate;
	}

	if (this->m_delta_yaw != 0.0f)
	{
		if (glm::abs(this->m_delta_yaw) < 0.01f)
		{
			this->m_yaw = std::roundf(this->m_yaw + this->m_delta_yaw);
			this->m_delta_yaw = 0.0f;
		}

		if (this->m_yaw < 0.0f)
			this->m_yaw += 360.0f;

		else if (this->m_yaw >= 360.0f)
			this->m_yaw -= 360.0f;
	}

	if (this->m_delta_pitch == 0.0f && this->m_delta_yaw == 0.0f)
		this->m_rotating = false;

	this->orientate();
}

void Camera::zoom(double p_yoffset)
{
	if (this->m_mode != FREE)
		return;
	
	this->m_zoom -= static_cast<float>(p_yoffset);

	if (this->m_zoom < 10.0f)
		this->m_zoom = 10.0f;

	else if (this->m_zoom > 45.0f)
		this->m_zoom = 45.0f;
}

// Moves camera forward or backward relative to its orientation
void Camera::advance(double p_yoffset)
{
	if (this->m_mode != FIXED)
		return;
	
	this->m_depth -= static_cast<float>(p_yoffset) * CAMERA_SENSITIVITY;

	if (this->m_depth < 2.2f)
		this->m_depth = 2.2f;

	else if (this->m_depth > 3.0f)
		this->m_depth = 3.0f;

	this->m_position = -this->m_front * this->m_depth;
}

// Interrupts camera's auto-rotation
void Camera::halt()
{
	if (!this->m_rotating)
		return;

	this->m_delta_pitch = 0.0f;
	this->m_delta_yaw = 0.0f;

	this->m_rotating = false;
}

// Rotates camera about screen axes
// Depending on camera mode, will either be about its own or world's origin
void Camera::orientate()
{
	glm::vec3 front;

	front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
	front.y = (this->m_mode == FIXED ? -1.0f : 1.0f) * sin(glm::radians(this->m_pitch));
	front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));

	this->m_front = glm::normalize(front);
	this->m_right = glm::normalize(glm::cross(this->m_front, WORLD_UP));
	this->m_up = glm::normalize(glm::cross(this->m_right, this->m_front));

	if (this->m_mode == FIXED)
		this->m_position = -front * this->m_depth;
}
