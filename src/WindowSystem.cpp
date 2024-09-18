#include "WindowSystem.h"

#include <AsyncInfo.h>
#include <iostream>

#include "HaglConstants.h"
#include "HaglUtility.h"

#include "imgui_impl_sdl2.h"
#include "imgui.h"
#include "imgui_internal.h"

tomway::WindowSystem::WindowSystem(unsigned const width, unsigned const height)
	: _mouse_visible(true),
	  _window_width(width),
	  _window_height(height)
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		LOG_ERROR("Could not initialize SDL video with error: %s", SDL_GetError());
		exit(1);
	}

	_window = SDL_CreateWindow(
		APP_NAME
		, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
		, 1024, 768
		, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

	_window_id = SDL_GetWindowID(_window);
	LOG_INFO("Window system initialized.");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	
	if (!ImGui_ImplSDL2_InitForVulkan(_window))
	{
		LOG_ERROR("ImGui failed to initialize SDL2 for Vulkan!");
	}

	LOG_INFO("Initialized ImGui for SDL2!");
}

tomway::WindowSystem::~WindowSystem()
{
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

vk::UniqueSurfaceKHR tomway::WindowSystem::create_vulkan_surface(const vk::Instance& instance) const
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(_window, instance, &surface);
	return vk::UniqueSurfaceKHR(surface);
}

std::vector<const char*> tomway::WindowSystem::get_extensions() const {
	unsigned int extensionCount;
	SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, nullptr);

	std::vector<const char*> extensions(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(_window, &extensionCount, extensions.data());

	return extensions;
}

bool tomway::WindowSystem::get_mouse_visible()
{
	return _mouse_visible;
}

void tomway::WindowSystem::get_vulkan_framebuffer_size(uint32_t& width, uint32_t& height) const {
	int w, h;
	SDL_Vulkan_GetDrawableSize(_window, &w, &h);

	if (w == NULL || w < 1 || h == NULL || h < 1) {
		throw std::runtime_error("Framebuffer size invalid.");
	}

	width = static_cast<uint32_t>(w);
	height = static_cast<uint32_t>(h);
}

std::vector<tomway::InputEvent> tomway::WindowSystem::handle_events() {
	SDL_Event e;
	std::vector<InputEvent> input_events;
	ImGui_ImplSDL2_NewFrame();

	while (SDL_PollEvent(&e)) {
		ImGui_ImplSDL2_ProcessEvent(&e);
		
		switch (e.type) {
		case SDL_WINDOWEVENT:
			if (e.window.windowID == _window_id) {
				switch (e.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					_window_width = e.window.data1;
					_window_height = e.window.data2;

					for (auto const& callback : _framebuffer_resize_callbacks) {
						callback();
					}

					break;
				case SDL_WINDOWEVENT_MINIMIZED:
					for (auto const& callback : _minimized_callbacks) {
						callback();
					}

					break;
				case SDL_WINDOWEVENT_CLOSE:
					e.type = SDL_QUIT;
					SDL_PushEvent(&e);
					break;
				default:;
				}
			}

			break;
		case SDL_QUIT:
			LOG_INFO("Received quit event, exiting!");
			exit(0);
		case SDL_KEYDOWN:  // NOLINT(clang-diagnostic-implicit-fallthrough)
		case SDL_KEYUP: {
			InputButton const button = to_input_button(e.key.keysym.sym);
			InputEventType const type = to_input_event_type(static_cast<SDL_EventType>(e.type));

			if (button != InputButton::NONE) {
				input_events.push_back({ type, button });
			}
		}
		case SDL_MOUSEBUTTONUP:  // NOLINT(clang-diagnostic-implicit-fallthrough)
		case SDL_MOUSEBUTTONDOWN: {
			InputButton const button = to_input_button(e.button);
			InputEventType const type = to_input_event_type(static_cast<SDL_EventType>(e.type));

			if (button != InputButton::NONE) {
				input_events.push_back({ type, button });
			}
			break;
		}
		case SDL_MOUSEMOTION:
			{
				InputButton const button = to_input_button(e.button);
				InputEventType const type = to_input_event_type(static_cast<SDL_EventType>(e.type));
				float const mouse_x = e.motion.xrel;
				float const mouse_y = e.motion.yrel;
				
				input_events.push_back({type, button, mouse_x, mouse_y});
			}
		default:
			break;
		}
	}

	if (not _mouse_visible)
	{
		SDL_WarpMouseInWindow(_window, _window_width / 2, _window_height / 2);
	}
	
	return input_events;
}

void tomway::WindowSystem::register_framebuffer_resize_callback(std::function<void()> const& callback) {
	_framebuffer_resize_callbacks.push_back(callback);
}

void tomway::WindowSystem::register_minimized_callback(std::function<void()> const& callback) {
	_minimized_callbacks.push_back(callback);
}

void tomway::WindowSystem::set_mouse_visible(bool mouse_visible)
{
	_mouse_visible = mouse_visible;
	SDL_SetRelativeMouseMode(_mouse_visible ? SDL_FALSE : SDL_TRUE);
}

void tomway::WindowSystem::toggle_mouse_visible()
{
	set_mouse_visible(!_mouse_visible);
}

void tomway::WindowSystem::wait_while_minimized() {
	auto flags = SDL_GetWindowFlags(_window);
	
	while (flags & SDL_WINDOW_MINIMIZED) {
		SDL_WaitEvent(nullptr);
		handle_events();
		flags = SDL_GetWindowFlags(_window);
	}
}
