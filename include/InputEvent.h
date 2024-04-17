#pragma once

#include "SDL.h"

namespace hagl {
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
	};

	struct InputEvent
	{
		InputEventType type;
		InputButton button;
		int mouseX;
		int mouseY;
	};

	InputEventType toInputEventType(SDL_EventType sdlEventType);
	InputButton toInputButton(SDL_Keycode sdlKeycode);
	InputButton toInputButton(SDL_MouseButtonEvent sdlMouseButtonEvent);
}