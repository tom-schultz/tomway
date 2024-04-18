// hagl.cpp : Defines the entry point for the application.
//

#include "Hagl.h"

#include <chrono>

#include "Cell.h"
#include "HaglUtility.h"
#include "RenderSystem.h"
#include "Vertex.h"
#include "WindowSystem.h"
#include "InputEvent.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main(int argc, char* argv[])
{
	std::vector<hagl::Cell> cells;

	for (float i = -4; i < 5; i++) {
		for (float j = -4; j < 5; j++) {
			cells.push_back({ i, j, (rand() % 2 == 0) });
		}
	}

	hagl::WindowSystem windowSystem(1024, 768);
	hagl::RenderSystem renderSystem(windowSystem, (uint32_t) cells.size() * hagl::Cell::verticesPerCell());
	auto startTime = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point lastTime = startTime;
	uint32_t width, height;
	glm::vec3 pos(0);
	
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;

	uint32_t frameAccumulator = 0, lastSecFrames = 0;
	float timer = 0;

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

		if (w) pos[1] += 1.0f * dt;
		if (s) pos[1] -= 1.0f * dt;
		if (a) pos[0] -= 1.0f * dt;
		if (d) pos[0] += 1.0f * dt;

		hagl::Transform transform;
		transform.model = glm::translate(transform.model, pos);
		//transform.model = glm::rotate(transform.model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		transform.view = glm::lookAt(
			glm::vec3(0.0f, 0.5f, 15.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

		transform.projection = glm::perspective(
			glm::radians(45.0f),
			width / (float)height,
			0.1f,
			1000.0f);

		std::vector<hagl::Vertex> vertices;
		vertices.reserve(cells.size() * hagl::Cell::verticesPerCell());

		for (auto cell : cells) {
			cell.getVertices(vertices);
		}

		renderSystem.drawFrame(transform, vertices, {});

		frameAccumulator += 1;
		timer += dt;

		if (timer > 1.0f) {
			LOG_INFO("FPS: %d", frameAccumulator - lastSecFrames);
			lastSecFrames = frameAccumulator;
			timer = 0;
		}
	}

	return 0;
}