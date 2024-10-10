#include "camera_controller.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "tomway_utility.h"
#include "Tracy.hpp"
#include "ui_system.h"
#include "input/input_system.h"

tomway::camera_controller::camera_controller(glm::vec3 initial_pos, float initial_vert_rot, float initial_hor_rot)
	: _pos(initial_pos),
	_pos_initial(initial_pos),
	_hor_rot(initial_hor_rot),
	_hor_rot_initial(initial_hor_rot),
	_vert_rot(initial_vert_rot),
	_vert_rot_initial(initial_vert_rot)
{
	update(0);
}

glm::mat4 tomway::camera_controller::get_projection_transform(uint32_t width, uint32_t height) const
{
    ZoneScoped;
	auto transform = glm::perspective(
			glm::radians(_fov),
			static_cast<float>(width) / static_cast<float>(height),
			_near_plane,
			_far_plane);
	
	transform[1][1] *= -1;
	return transform;
}

glm::mat4 tomway::camera_controller::get_view_transform() const
{
    ZoneScoped;
	
	auto const ret = glm::lookAt(
        _pos,
        _pos + _fwd,
        _up);

	return ret;
}

void tomway::camera_controller::reset()
{
    ZoneScoped;
	_pos = _pos_initial;
	_hor_rot = _hor_rot_initial;
	_vert_rot = _vert_rot_initial;
}

void tomway::camera_controller::update(float time_delta)
{
    ZoneScoped;
	glm::vec2 mouse_delta = input_system::get_mouse_delta();

	if (mouse_delta != glm::vec2())
	{
		mouse_delta = glm::normalize(mouse_delta);
	}
	
	// Inverted
	auto vert_delta = mouse_delta.y * time_delta * 180.0f;
	_vert_rot -= vert_delta;
	auto hor_delta = mouse_delta.x * time_delta * 180.0f;
	_hor_rot -= hor_delta;
	ui_system::add_debug_text(string_format("Mouse Rotation: (%f, %f)", _hor_rot, _vert_rot));
	if (input_system::btn_down(input_button::W)) _pos += _fwd * _speed * time_delta;
	if (input_system::btn_down(input_button::S)) _pos -= _fwd * _speed * time_delta;
	if (input_system::btn_down(input_button::A)) _pos -= _right * _speed * time_delta;
	if (input_system::btn_down(input_button::D)) _pos += _right * _speed * time_delta;
	
	glm::mat4 rotation_mat(1.0f);
	rotation_mat = glm::rotate(rotation_mat, glm::radians(_vert_rot), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation_mat = glm::rotate(rotation_mat, glm::radians(_hor_rot), glm::vec3(0.0f, 0.0f, 1.0f));
	
	_right = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) * rotation_mat;
	_fwd = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * rotation_mat;
	_up = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * rotation_mat;
}
