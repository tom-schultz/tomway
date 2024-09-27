#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "InputEvent.h"
#include "SDL_vulkan.h"

namespace tomway {
	using sdl_ui_init_fn = bool(*)(SDL_Window*);
	using sdl_ui_event_handler_fn = bool(*)(SDL_Event const*);
	
	class WindowSystem
	{
	public:
		WindowSystem(unsigned width, unsigned height);
		~WindowSystem();
		vk::UniqueSurfaceKHR create_vulkan_surface(vk::Instance const& instance) const;
		std::vector<const char*> get_extensions() const;
		bool get_mouse_visible();
		void get_vulkan_framebuffer_size(uint32_t& width, uint32_t& height) const;
		std::vector<InputEvent> handle_events();
		bool init_ui_sdl(sdl_ui_init_fn init_fn, sdl_ui_event_handler_fn event_handler_fn);
		void register_framebuffer_resize_callback(std::function<void()> const& callback);
		void register_minimized_callback(std::function<void()> const& callback);
		void set_mouse_visible(bool mouse_visible);
		void toggle_mouse_visible();
		void wait_while_minimized();
	private:
		std::vector<std::function<void()>> _framebuffer_resize_callbacks;
		std::vector<std::function<void()>> _minimized_callbacks;
		bool _mouse_visible = false;
		sdl_ui_event_handler_fn _sdl_ui_event_handler_fn;
		SDL_Window* _window = nullptr;
		unsigned _window_width, _window_height;
		uint32_t _window_id;
	};
}