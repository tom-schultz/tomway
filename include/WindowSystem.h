#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "InputEvent.h"
#include "SDL_vulkan.h"

namespace tomway {
	class WindowSystem
	{
	public:
		WindowSystem(unsigned width, unsigned height);
		vk::UniqueSurfaceKHR create_vulkan_surface(vk::Instance const& instance) const;
		std::vector<const char*> get_extensions() const;
		void get_vulkan_framebuffer_size(uint32_t& width, uint32_t& height) const;
		std::vector<InputEvent> handle_events();
		void register_framebuffer_resize_callback(std::function<void()> const& callback);
		void register_minimized_callback(std::function<void()> const& callback);
		void wait_while_minimized();
	private:
		std::vector<std::function<void()>> _framebuffer_resize_callbacks;
		std::vector<std::function<void()>> _minimized_callbacks;
		SDL_Window* _window = nullptr;
		unsigned _window_width, _window_height;
		uint32_t _window_id;
	};
}