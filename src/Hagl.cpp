// hagl.cpp : Defines the entry point for the application.
//

#include <chrono>

#include "Hagl.h"
#include "HaglUtility.h"
#include "RenderSystem.h"
#include "Vertex.h"
#include "WindowSystem.h"
#include "InputEvent.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
	auto startTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point lastTime = startTime;
	uint32_t width, height;
	glm::vec3 pos(0);
	
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;

	while (true) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		auto inputEvents = windowSystem.handle_events();
		windowSystem.getVulkanFramebufferSize(width, height);

		for (auto event : inputEvents) {
			switch (event.button) {
			case hagl::InputButton::W:
				w = event.type == hagl::InputEventType::BUTTON_DOWN;
				break;
			case hagl::InputButton::A:
				a = event.type == hagl::InputEventType::BUTTON_DOWN;
				break;
			case hagl::InputButton::S:
				s = event.type == hagl::InputEventType::BUTTON_DOWN;
				break;
			case hagl::InputButton::D:
				d = event.type == hagl::InputEventType::BUTTON_DOWN;
				break;
			}
		}

		if (w) pos[1] -= 1.0f * dt;
		if (s) pos[1] += 1.0f * dt;
		if (a) pos[0] += 1.0f * dt;
		if (d) pos[0] -= 1.0f * dt;

		hagl::Transform transform;
		transform.model = glm::translate(transform.model, pos);
		transform.model = glm::rotate(transform.model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		transform.view = glm::lookAt(
			glm::vec3(0.0f, 1.00000f, 6.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

		transform.projection = glm::perspective(
			glm::radians(45.0f),
			width / (float)height,
			0.1f,
			1000.0f);

		renderSystem.drawFrame(transform, vertices, {});
	}

	return 0;
}