/* ============================================================================
 * Project:	Gungi3D
 * 
 * File:	Camera.h
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

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>

// World properties
static const glm::vec3 WORLD_ORIGIN = glm::vec3(0.0f);
static const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

// Camera properties
static const float CAMERA_SPEED = 5.0f;
static const float CAMERA_SENSITIVITY = 0.1f;

class Camera
{
public:
	enum Mode { FIXED, FREE, };
	enum Direction { FORWARD, BACKWARD, LEFT, RIGHT, };

	// Member functions
	// ----------------
	inline Mode getMode() { return this->m_mode; }

	inline glm::mat4 getView() { return glm::lookAt(this->m_position, (this->m_mode == FREE ? this->m_position + this->m_front : WORLD_ORIGIN), WORLD_UP); }

	inline glm::vec3 getPosition() { return this->m_position; }
	inline glm::vec3 getFront() { return this->m_front; }

	inline glm::vec2 getRotation() { return glm::vec2(this->m_pitch, this->m_yaw); }

	inline float getZoom() { return this->m_zoom; }

	inline bool getRotatable() { return this->m_rotatable; }
	inline bool getRotating() { return this->m_rotating; }

	void init(Mode p_mode);

	void setRotatable(bool p_rotatable); // Allows camera to rotate about world's origin based on mouse movements
	void setRotation(float p_pitch, float p_yaw); // Sets camera to auto-rotate about world's origin towards specified angles

	void adjustPitch(bool p_sign);
	void adjustYaw(bool p_sign);

	void move(Direction p_direction, double p_delta_time); // Changes camera's position based on directional input

	void turn(double p_xoffset, double p_yoffset); // Changes camera's orientation about it's own origin based on mouse movements
	void rotate(double p_xoffset, double p_yoffset); // Changes camera's orientation and position about world's origin based on mouse movements
	void autoRotate(double p_delta_time); // Advances camera incrementally about world's origin towards its desired orientation and position

	void zoom(double p_yoffset);
	void advance(double p_yoffset); // Moves camera forward or backward relative to its orientation

private:
	void halt(); // Interrupts camera's auto-rotation

	void orientate(); // Rotates camera about screen axes

	// Member variables
	// ----------------
	Mode m_mode;
	
	glm::vec3 m_position;

	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;

	float m_pitch;
	float m_yaw;

	float m_delta_pitch; // Remaining angle to reach desired pitch
	float m_delta_yaw; // Remaining angle to reach desired yaw

	float m_zoom;
	float m_depth;

	bool m_rotatable; // Camera can rotate with mouse movement
	bool m_rotating; // Camera is auto-rotating
};

#endif // CAMERA_H
