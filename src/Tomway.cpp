// tomway.cpp : Defines the entry point for the application.
//

#include "Tomway.h"
#include "HaglUtility.h"
#include "InputEvent.h"
#include "RenderSystem.h"
#include "SimulationSystem.h"
#include "TimeSystem.h"
#include "WindowSystem.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

size_t constexpr GRID_SIZE = 10;

int main(int argc, char* argv[])
{
	tomway::SimulationSystem simulation_system(GRID_SIZE);	
	tomway::CellGeometry cell_geometry_generator(simulation_system.current_cells());
	tomway::WindowSystem window_system(1024, 768);
	tomway::RenderSystem render_system(window_system, cell_geometry_generator);

	tomway::TimeSystem time_system(5);
	float delta = 0;
	
	uint32_t width, height;
	glm::vec3 model_pos(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_pos(0, 0, GRID_SIZE);
	
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
	bool esc = false;
	bool step = false;
	bool locked = true;

	glm::vec3 fwd = {0, 0, 0};
	float vert_rot = -90.0f;
	float hor_rot = 0.01f;
	
	while (true) {
		delta = time_system.new_frame();
		auto input_events = window_system.handle_events();
		window_system.get_vulkan_framebuffer_size(width, height);
		float mouse_x = 0, mouse_y = 0;

		for (const auto event : input_events) {
			if (event.type == tomway::InputEventType::MOUSE_MOTION)
			{
				mouse_x = event.mouse_x;
				mouse_y = event.mouse_y;
			}
			else if (event.type == tomway::InputEventType::BUTTON_DOWN || event.type == tomway::InputEventType::BUTTON_UP)
			{
				switch (event.button) {  // NOLINT(clang-diagnostic-switch-enum)
				case tomway::InputButton::W:
					w = event.type == tomway::InputEventType::BUTTON_DOWN;
					break;
				case tomway::InputButton::A:
					a = event.type == tomway::InputEventType::BUTTON_DOWN;
					break;
				case tomway::InputButton::S:
					s = event.type == tomway::InputEventType::BUTTON_DOWN;
					break;
				case tomway::InputButton::D:
					d = event.type == tomway::InputEventType::BUTTON_DOWN;
					break;
				case tomway::InputButton::ESCAPE:
					esc = true;
					break;
				case tomway::InputButton::SPACE:
					if (event.type == tomway::InputEventType::BUTTON_DOWN)
					{
						step = true;
					}
					break;
				case tomway::InputButton::L:
					if (event.type == tomway::InputEventType::BUTTON_DOWN)
					{
						locked = !locked;
					}
				
					break;
				default:;
				}
			}
		}
		
		glm::vec3 right = glm::vec3(
			cos(glm::radians(hor_rot)),
			sin(glm::radians(hor_rot)),
			0
		);
		
		fwd[0] = cos(glm::radians(vert_rot)) * sin(glm::radians(hor_rot));
		fwd[1] = cos(glm::radians(vert_rot)) * cos(glm::radians(hor_rot));
		fwd[2] = sin(glm::radians(vert_rot));
		
		glm::vec3 up = glm::cross(right, fwd);

		// Inverted
		vert_rot += mouse_y * delta * 180.0f;
		hor_rot += mouse_x * delta * 180.0f;
		
		float constexpr speed = 4.0f;
		
		if (w) camera_pos += fwd * speed * delta;
		if (s) camera_pos -= fwd * speed * delta;
		if (a) camera_pos -= right * speed * delta;
		if (d) camera_pos += right * speed * delta;

		if (esc)
		{
			break;	
		}

		tomway::Transform transform;
		transform.model = glm::translate(transform.model, model_pos);

		transform.view = glm::lookAt(
			camera_pos,
			camera_pos + fwd,
			up);

		transform.projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(width) / static_cast<float>(height),
			0.1f,
			1000.0f);

		transform.projection[1][1] *= -1;
		
		if ((!locked && time_system.get_new_tick()) || step)
		{
			auto cells = simulation_system.step_simulation();
			cell_geometry_generator.bind_cells(cells);
			step = false;
		}
		
		render_system.draw_frame(transform);
	}

	exit(0);
}
