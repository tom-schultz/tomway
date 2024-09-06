#include "InputEvent.h"

tomway::InputEventType tomway::to_input_event_type(SDL_EventType const& sdl_event_type) {
	switch (sdl_event_type)  // NOLINT(clang-diagnostic-switch-enum)
	{
	case SDL_KEYUP:
		return InputEventType::BUTTON_UP;
	case SDL_KEYDOWN:
		return InputEventType::BUTTON_DOWN;
	case SDL_MOUSEBUTTONUP:
		return InputEventType::BUTTON_UP;
	case SDL_MOUSEBUTTONDOWN:
		return InputEventType::BUTTON_DOWN;
	case SDL_MOUSEMOTION:
		return InputEventType::MOUSE_MOTION;
	default:
		return InputEventType::UNSUPPORTED;
	}
}

tomway::InputButton tomway::to_input_button(SDL_Keycode const& sdl_keycode) {
	switch (sdl_keycode) {
	case SDLK_w:
		return InputButton::W;
	case SDLK_a:
		return InputButton::A;
	case SDLK_s:
		return InputButton::S;
	case SDLK_d:
		return InputButton::D;
	case SDLK_l:
		return InputButton::L;
	case SDLK_r:
		return InputButton::R;
	case SDLK_ESCAPE:
		return InputButton::ESCAPE;
	case SDLK_SPACE:
		return InputButton::SPACE;
	case SDLK_F1:
		return InputButton::F1;
	default:
		return InputButton::NONE;
	}
}

tomway::InputButton tomway::to_input_button(SDL_MouseButtonEvent const& sdl_mouse_button_event) {
	switch (sdl_mouse_button_event.button) {
		case SDL_BUTTON_LEFT:
		return InputButton::MOUSE_LEFT;
	case SDL_BUTTON_RIGHT:
		return InputButton::MOUSE_RIGHT;
	default:
		return InputButton::NONE;
	}
}