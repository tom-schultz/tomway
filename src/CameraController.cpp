#include "CameraController.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "input/InputSystem.h"

tomway::CameraController::CameraController(glm::vec3 initial_pos, float initial_vert_rot, float initial_hor_rot)
	: _pos(initial_pos),
	_pos_initial(initial_pos),
	_hor_rot(initial_hor_rot),
	_hor_rot_initial(initial_hor_rot),
	_vert_rot(initial_vert_rot),
	_vert_rot_initial(initial_vert_rot)
{
}

glm::mat4 tomway::CameraController::get_projection_transform(uint32_t width, uint32_t height) const
{
	auto transform = glm::perspective(
			glm::radians(_fov),
			static_cast<float>(width) / static_cast<float>(height),
			_near_plane,
			_far_plane);
	
	transform[1][1] *= -1;
	return transform;
}

glm::mat4 tomway::CameraController::get_view_transform() const
{
	return glm::lookAt(
        _pos,
        _pos + _fwd,
        _up);
}

void tomway::CameraController::reset()
{
	_pos = _pos_initial;
	_hor_rot = _hor_rot_initial;
	_vert_rot = _vert_rot_initial;
}

void tomway::CameraController::update(float delta)
{
	glm::vec2 const mouse_vel = InputSystem::get_mouse_vel();
				
	// Inverted
	_vert_rot -= mouse_vel.y * delta * 180.0f;
	_hor_rot -= mouse_vel.x * delta * 180.0f;
		
	if (InputSystem::btn_down(InputButton::W)) _pos += _fwd * _speed * delta;
	if (InputSystem::btn_down(InputButton::S)) _pos -= _fwd * _speed * delta;
	if (InputSystem::btn_down(InputButton::A)) _pos -= _right * _speed * delta;
	if (InputSystem::btn_down(InputButton::D)) _pos += _right * _speed * delta;
	
	glm::mat4 rotation_mat(1.0f);
	rotation_mat = glm::rotate(rotation_mat, glm::radians(_vert_rot), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation_mat = glm::rotate(rotation_mat, glm::radians(_hor_rot), glm::vec3(0.0f, 0.0f, 1.0f));
	
	_right = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) * rotation_mat;
	_fwd = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * rotation_mat;
	_up = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * rotation_mat;
}
