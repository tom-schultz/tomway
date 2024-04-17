#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "InputEvent.h"
#include "SDL.h"
#include "SDL_vulkan.h"

namespace hagl {
	class WindowSystem
	{
	public:
		WindowSystem(unsigned width, unsigned height);
		vk::UniqueSurfaceKHR createVulkanSurface(const vk::Instance& instance);
		std::vector<const char*> getExtensions() const;
		void getVulkanFramebufferSize(uint32_t& width, uint32_t& height) const;
		std::vector<InputEvent> handle_events();
		void registerFramebufferResizeCallback(std::function<void()> callback);
		void registerMinimizedCallback(std::function<void()> callback);
		void waitWhileMinimized();
	private:
		std::vector<std::function<void()>> _framebufferResizeCallbacks;
		std::vector<std::function<void()>> _minimizedCallbacks;
		SDL_Window* _window = NULL;
		unsigned _windowHeight, _windowWidth;
		uint32_t _windowId;
		SDL_Surface* _windowSurface = NULL;
	};
}