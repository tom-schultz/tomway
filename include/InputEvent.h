#pragma once

#include "SDL.h"

namespace tomway {
	enum InputEventType {
		UNSUPPORTED = 0,
		BUTTON_DOWN,
		BUTTON_UP
	};

	enum InputButton {
		NONE = 0,
		W,
		A,
		S,
		D,
		ESCAPE,
		MOUSE_LEFT,
		MOUSE_RIGHT,
		SPACE,
		L,
	};

	struct InputEvent
	{
		InputEventType type;
		InputButton button;
		int mouseX;
		int mouseY;
	};

	InputEventType to_input_event_type(SDL_EventType const& sdl_event_type);
	InputButton to_input_button(SDL_Keycode const& sdl_keycode);
	InputButton to_input_button(SDL_MouseButtonEvent const& sdl_mouse_button_event);
}