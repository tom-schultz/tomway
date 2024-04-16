// hagl.cpp : Defines the entry point for the application.
//

#include "Hagl.h"
#include "RenderSystem.h"
#include "Vertex.h"
#include "WindowSystem.h"

const std::vector<hagl::Vertex> vertices = {
{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

int main(int argc, char* argv[])
{

	hagl::WindowSystem windowSystem(1024, 768);
	hagl::RenderSystem renderSystem(windowSystem, vertices.size());

	while (true) {
		windowSystem.handle_events();
		renderSystem.drawFrame({}, vertices, {});
	}

	return 0;
}