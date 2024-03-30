#include "WindowSystem.h"
#include <iostream>
#include "HaglConstants.h"

hagl::WindowSystem::WindowSystem(unsigned width, unsigned height) : _windowWidth(width), _windowHeight(height) {

}

void hagl::WindowSystem::init() {
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		LOG_ERROR(0, "Could not initialize SDL video with error: %s", SDL_GetError());
		exit(1);
	}

	_window = SDL_CreateWindow(
		APP_NAME
		, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
		, 1024, 768
		, SDL_WINDOW_VULKAN);

	LOG_INFO("Window system initialized.");
}

void hagl::WindowSystem::createVulkanSurface(vk::Instance instance, VkSurfaceKHR& surface) {
	SDL_Vulkan_CreateSurface(_window, instance, &surface);
}

std::vector<const char*> hagl::WindowSystem::getExtensions() const {
	unsigned int extensionCount;
	SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, NULL);

	std::vector<const char*> extensions(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, extensions.data());

	return extensions;
}

void hagl::WindowSystem::handle_events() {
	SDL_Event e;

	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			exit(0);
		}
	}
}

void hagl::WindowSystem::getVulkanFramebufferSize(uint32_t& width, uint32_t& height) const {
	int w, h;
	SDL_Vulkan_GetDrawableSize(_window, &w, &h);

	if (w == NULL || w < 1 || h == NULL || h < 1) {
		throw new std::runtime_error("Framebuffer size invalid.");
	}

	width = (uint32_t) w;
	height = (uint32_t) h;
}