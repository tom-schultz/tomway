#pragma once

#include "SDL.h"
#include "SDL_vulkan.h"
#include <vector>
#include "vulkan/vulkan.hpp"
#include <functional>

namespace hagl {
	class WindowSystem
	{
	public:
		WindowSystem(unsigned width, unsigned height);
		std::vector<const char*> getExtensions() const;
		void handle_events();
		vk::UniqueSurfaceKHR createVulkanSurface(const vk::Instance& instance);
		void getVulkanFramebufferSize(uint32_t& width, uint32_t& height) const;
		void registerFramebufferResizeCallback(std::function<void()> callback);
		void registerMinimizedCallback(std::function<void()> callback);
		void waitWhileMinimized();
	private:
		std::vector<std::function<void()>> _framebufferResizeCallbacks;
		std::vector<std::function<void()>> _minimizedCallbacks;
		SDL_Window* _window = NULL;
		uint32_t _windowId;
		SDL_Surface* _windowSurface = NULL;
		unsigned  _windowWidth;
		unsigned _windowHeight;
	};
}