#include "WindowSystem.h"
#include <iostream>
#include "HaglConstants.h"
#include "HaglUtility.h"

hagl::WindowSystem::WindowSystem(unsigned width, unsigned height)
	: _windowWidth(width),
	_windowHeight(height)
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		LOG_ERROR(0, "Could not initialize SDL video with error: %s", SDL_GetError());
		exit(1);
	}

	_window = SDL_CreateWindow(
		APP_NAME
		, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
		, 1024, 768
		, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

	_windowId = SDL_GetWindowID(_window);
	LOG_INFO("Window system initialized.");
}

vk::UniqueSurfaceKHR hagl::WindowSystem::createVulkanSurface(const vk::Instance& instance) {
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(_window, instance, &surface);
	return vk::UniqueSurfaceKHR(surface);
}

std::vector<const char*> hagl::WindowSystem::getExtensions() const {
	unsigned int extensionCount;
	SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, NULL);

	std::vector<const char*> extensions(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, extensions.data());

	return extensions;
}

void hagl::WindowSystem::getVulkanFramebufferSize(uint32_t& width, uint32_t& height) const {
	int w, h;
	SDL_Vulkan_GetDrawableSize(_window, &w, &h);

	if (w == NULL || w < 1 || h == NULL || h < 1) {
		throw new std::runtime_error("Framebuffer size invalid.");
	}

	width = (uint32_t)w;
	height = (uint32_t)h;
}

std::vector<hagl::InputEvent> hagl::WindowSystem::handle_events() {
	SDL_Event e;
	std::vector<InputEvent> inputEvents;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_WINDOWEVENT:
			if (e.window.windowID == _windowId) {
				switch (e.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					_windowWidth = e.window.data1;
					_windowHeight = e.window.data2;

					for (auto callback : _framebufferResizeCallbacks) {
						callback();
					}

					break;
				case SDL_WINDOWEVENT_MINIMIZED:
					for (auto callback : _minimizedCallbacks) {
						callback();
					}

					break;
				case SDL_WINDOWEVENT_CLOSE:
					e.type = SDL_QUIT;
					SDL_PushEvent(&e);
					break;
				default:
					continue;
				}
			}

			break;
		case SDL_QUIT:
			LOG_INFO("Received quit event, exiting!");
			exit(0);
		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			InputButton button = toInputButton(e.key.keysym.sym);
			InputEventType type = toInputEventType((SDL_EventType)e.type);

			if (button != InputButton::NONE) {
				inputEvents.push_back({ type, button });
			}
		}
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN: {
			InputButton button = toInputButton(e.button);
			InputEventType type = toInputEventType((SDL_EventType)e.type);

			if (button != InputButton::NONE) {
				inputEvents.push_back({ type, button });
			}
			break;
		}
		default:
			break;
		}
	}

	return inputEvents;
}

void hagl::WindowSystem::registerFramebufferResizeCallback(std::function<void()> callback) {
	_framebufferResizeCallbacks.push_back(callback);
}

void hagl::WindowSystem::registerMinimizedCallback(std::function<void()> callback) {
	_minimizedCallbacks.push_back(callback);
}

void hagl::WindowSystem::waitWhileMinimized() {
	auto flags = SDL_GetWindowFlags(_window);
	
	while (flags & SDL_WINDOW_MINIMIZED) {
		SDL_WaitEvent(nullptr);
		handle_events();
		flags = SDL_GetWindowFlags(_window);
	}
}