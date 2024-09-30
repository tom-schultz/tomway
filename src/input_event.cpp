#include "input_event.h"

tomway::input_event_type tomway::to_input_event_type(SDL_EventType const& sdl_event_type) {
	switch (sdl_event_type)  // NOLINT(clang-diagnostic-switch-enum)
	{
	case SDL_KEYUP:
		return input_event_type::BUTTON_UP;
	case SDL_KEYDOWN:
		return input_event_type::BUTTON_DOWN;
	case SDL_MOUSEBUTTONUP:
		return input_event_type::BUTTON_UP;
	case SDL_MOUSEBUTTONDOWN:
		return input_event_type::BUTTON_DOWN;
	case SDL_MOUSEMOTION:
		return input_event_type::MOUSE_MOTION;
	default:
		return input_event_type::UNSUPPORTED;
	}
}

tomway::input_button tomway::to_input_button(SDL_Keycode const& sdl_keycode) {
	switch (sdl_keycode) {
	case SDLK_w:
		return input_button::W;
	case SDLK_a:
		return input_button::A;
	case SDLK_s:
		return input_button::S;
	case SDLK_d:
		return input_button::D;
	case SDLK_l:
		return input_button::L;
	case SDLK_p:
		return input_button::P;
	case SDLK_r:
		return input_button::R;
	case SDLK_ESCAPE:
		return input_button::ESCAPE;
	case SDLK_SPACE:
		return input_button::SPACE;
	case SDLK_F1:
		return input_button::F1;
	case SDLK_F2:
		return input_button::F2;
	case SDLK_F3:
		return input_button::F3;
	default:
		return input_button::NONE;
	}
}

tomway::input_button tomway::to_input_button(SDL_MouseButtonEvent const& sdl_mouse_button_event) {
	switch (sdl_mouse_button_event.button) {
		case SDL_BUTTON_LEFT:
		return input_button::MOUSE_LEFT;
	case SDL_BUTTON_RIGHT:
		return input_button::MOUSE_RIGHT;
	default:
		return input_button::NONE;
	}
}