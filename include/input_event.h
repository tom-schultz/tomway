#pragma once

#include "SDL.h"

namespace tomway {
	enum class input_event_type {
		UNSUPPORTED = 0,
		BUTTON_DOWN,
		BUTTON_UP,
		MOUSE_MOTION
	};

	enum class input_button {
		NONE = 0,
		W,
		A,
		S,
		D,
		L,
		P,
		R,
		ESCAPE,
		MOUSE_LEFT,
		MOUSE_RIGHT,
		SPACE,
		F1,
		F2,
		F3,
	};

	struct input_event
	{
		input_event_type type;
		input_button button;
		float mouse_x_vel = 0;
		float mouse_y_vel = 0;
	};

	input_event_type to_input_event_type(SDL_EventType const& sdl_event_type);
	input_button to_input_button(SDL_Keycode const& sdl_keycode);
	input_button to_input_button(SDL_MouseButtonEvent const& sdl_mouse_button_event);
}