#pragma once

#include "SDL.h"
#include "SDL_vulkan.h"
#include <vector>
#include "vulkan/vulkan.hpp"

namespace hagl {
	class WindowSystem
	{
	public:
		WindowSystem(unsigned width, unsigned height);
		void init();
		std::vector<const char*> getExtensions() const;
		void handle_events();
		vk::UniqueSurfaceKHR createVulkanSurface(const vk::Instance& instance);
		void getVulkanFramebufferSize(uint32_t& width, uint32_t& height) const;
	private:
		SDL_Window* _window = NULL;
		SDL_Surface* _windowSurface = NULL;
		unsigned  _windowWidth;
		unsigned _windowHeight;
	};
}