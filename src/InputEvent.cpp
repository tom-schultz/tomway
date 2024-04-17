#include "InputEvent.h"

hagl::InputEventType hagl::toInputEventType(SDL_EventType sdlEventType) {
	switch (sdlEventType)
	{
	case SDL_KEYUP:
		return InputEventType::BUTTON_UP;
	case SDL_KEYDOWN:
		return InputEventType::BUTTON_DOWN;
	case SDL_MOUSEBUTTONUP:
		return InputEventType::BUTTON_UP;
	case SDL_MOUSEBUTTONDOWN:
		return InputEventType::BUTTON_DOWN;
	default:
		return InputEventType::UNSUPPORTED;
	}
}

hagl::InputButton hagl::toInputButton(SDL_Keycode sdlKeycode) {
	switch (sdlKeycode) {
	case SDLK_w:
		return InputButton::W;
	case SDLK_a:
		return InputButton::A;
	case SDLK_s:
		return InputButton::S;
	case SDLK_d:
		return InputButton::D;
	case SDLK_ESCAPE:
		return InputButton::ESCAPE;
	default:
		return InputButton::NONE;
	}
}

hagl::InputButton hagl::toInputButton(SDL_MouseButtonEvent sdlMouseButtonEvent) {
	switch (sdlMouseButtonEvent.button) {
		case SDL_BUTTON_LEFT:
		return InputButton::MOUSE_LEFT;
	case SDL_BUTTON_RIGHT:
		return InputButton::MOUSE_RIGHT;
	default:
		return InputButton::NONE;
	}
}