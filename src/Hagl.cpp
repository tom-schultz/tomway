// hagl.cpp : Defines the entry point for the application.
//

#include "Hagl.h"
#include "HaglUtility.h"
#include "RenderSystem.h"
#include "Vertex.h"
#include "WindowSystem.h"

std::vector<hagl::Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
};

int main(int argc, char* argv[])
{
	hagl::WindowSystem windowSystem(1024, 768);
	hagl::RenderSystem renderSystem(windowSystem, (uint32_t) vertices.size());

	while (true) {
		auto inputEvents = windowSystem.handle_events();

		for (auto event : inputEvents) {
			LOG_INFO("Received input event of type %d and button %d", event.type, event.button);
		}

		renderSystem.drawFrame({}, vertices, {});
	}

	return 0;
}