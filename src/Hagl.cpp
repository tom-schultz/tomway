// hagl.cpp : Defines the entry point for the application.
//

#include "Hagl.h"


#include "Cell.h"
#include "HaglUtility.h"
#include "RenderSystem.h"
#include "Vertex.h"
#include "WindowSystem.h"
#include "InputEvent.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TimeSystem.h"

int main(int argc, char* argv[])
{
	std::vector<hagl::Cell> cells;

	for (int i = -4; i < 5; i++) {
		for (int j = -4; j < 5; j++) {
			cells.emplace_back(i, j, rand() % 2 == 0);
		}
	}

	hagl::WindowSystem window_system(1024, 768);
	
	const auto vert_count = static_cast<uint32_t>(cells.size()) * hagl::Cell::vertices_per_cell();
	hagl::RenderSystem render_system(window_system, vert_count);

	hagl::TimeSystem time_system {};
	float delta = 0;
	
	uint32_t width, height;
	glm::vec3 pos(0);
	
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
	bool esc = false;
	
	std::vector<hagl::Vertex> vertices;
	vertices.resize(cells.size() * hagl::Cell::vertices_per_cell());

	while (true) {
		delta = time_system.new_frame();
		auto input_events = window_system.handle_events();
		window_system.get_vulkan_framebuffer_size(width, height);

		for (const auto event : input_events) {
			switch (event.button) {  // NOLINT(clang-diagnostic-switch-enum)
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
			case hagl::InputButton::ESCAPE:
				esc = true;
				break;
			default:;
			}
		}

		if (w) pos[1] += 1.0f * delta;
		if (s) pos[1] -= 1.0f * delta;
		if (a) pos[0] -= 1.0f * delta;
		if (d) pos[0] += 1.0f * delta;

		if (esc)
		{
			break;	
		}

		hagl::Transform transform;
		transform.model = glm::translate(transform.model, pos);
		//transform.model = glm::rotate(transform.model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		transform.view = glm::lookAt(
			glm::vec3(0.0f, 0.5f, 15.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));

		transform.projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(width) / static_cast<float>(height),
			0.1f,
			1000.0f);

		size_t vertex_count = 0;
		vertices.resize(vertices.capacity());
		
		for (auto cell : cells) {
			cell.get_vertices(vertices, vertex_count);
		}

		vertices.resize(vertex_count);
		render_system.draw_frame(transform, vertices, {});
	}

	exit(0);
}
