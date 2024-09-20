#pragma once

#include "SDL.h"

namespace tomway {
	enum class InputEventType {
		UNSUPPORTED = 0,
		BUTTON_DOWN,
		BUTTON_UP,
		MOUSE_MOTION
	};

	enum class InputButton {
		NONE = 0,
		W,
		A,
		S,
		D,
		L,
		R,
		ESCAPE,
		MOUSE_LEFT,
		MOUSE_RIGHT,
		SPACE,
		F1,
	};

	struct InputEvent
	{
		InputEventType type;
		InputButton button;
		float mouse_x_vel = 0;
		float mouse_y_vel = 0;
	};

	InputEventType to_input_event_type(SDL_EventType const& sdl_event_type);
	InputButton to_input_button(SDL_Keycode const& sdl_keycode);
	InputButton to_input_button(SDL_MouseButtonEvent const& sdl_mouse_button_event);
}