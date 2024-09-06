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

#include "imgui.h"

size_t constexpr GRID_SIZE = 100;

int main(int argc, char* argv[])
{
	tomway::SimulationSystem simulation_system;	
	tomway::CellGeometry cell_geometry_generator(simulation_system.current_cells());
	tomway::WindowSystem window_system(1024, 768);
	
	tomway::RenderSystem render_system(
		window_system,
		cell_geometry_generator,
		tomway::CellGeometry::max_vertex_count(GRID_SIZE * GRID_SIZE));

	tomway::TimeSystem time_system(5);
	float delta = 0;
	
	uint32_t width, height;
	glm::vec3 model_pos(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_pos(0, 0, GRID_SIZE);
	
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
	bool r = false;
	bool esc = false;
	bool step = false;
	bool locked = true;
	bool main_menu = true;

	glm::vec3 fwd = {0, 0, 0};
	float vert_rot = -90.0f;
	float hor_rot = 0.01f;
	
	while (true) {
		auto input_events = window_system.handle_events();
		render_system.new_frame();
		window_system.get_vulkan_framebuffer_size(width, height);
		float mouse_x = 0, mouse_y = 0;
		
		delta = time_system.new_frame();
		
		glm::vec3 right = glm::vec3(
			cos(glm::radians(hor_rot)),
			sin(glm::radians(hor_rot)),
			0
		);
		
		glm::vec3 up = glm::cross(right, fwd);
		
		fwd[0] = cos(glm::radians(vert_rot)) * sin(glm::radians(hor_rot));
		fwd[1] = cos(glm::radians(vert_rot)) * cos(glm::radians(hor_rot));
		fwd[2] = sin(glm::radians(vert_rot));
		
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
				case tomway::InputButton::R:
					r = event.type == tomway::InputEventType::BUTTON_UP;
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
				case tomway::InputButton::F1:
					if (event.type == tomway::InputEventType::BUTTON_DOWN)
					{
						window_system.toggle_mouse();
					}
					
					break;
				default: break;
				}
			}
		}
		
		if (esc)
		{
			break;	
		}

		if (r)
		{
			r = false;
			simulation_system.start(0);
			main_menu = true;
			locked = true;
		}
		else if (main_menu)
		{
			draw_main_menu(&main_menu);
			
			if (not main_menu)
			{
				simulation_system.start(GRID_SIZE);
			}
		}
		else
		{
			if (not window_system.get_mouse_visible())
			{
				// Inverted
				vert_rot += mouse_y * delta * 180.0f;
				hor_rot += mouse_x * delta * 180.0f;
		
				float constexpr speed = 4.0f;
		
				if (w) camera_pos += fwd * speed * delta;
				if (s) camera_pos -= fwd * speed * delta;
				if (a) camera_pos -= right * speed * delta;
				if (d) camera_pos += right * speed * delta;
			}
		
			if ((!locked && time_system.get_new_tick()) || step)
			{
				simulation_system.step_simulation();
				step = false;
			}
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
		auto cells = simulation_system.current_cells();
		cell_geometry_generator.bind_cells(cells);
		render_system.draw_frame(transform);
	}

	exit(0);
}

void draw_main_menu(bool* main_menu)
{
	ImGuiWindowFlags constexpr window_flags =
		ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoBackground;

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Main Menu", main_menu, window_flags);

	
	if (ImGui::Button("Start", { 200, 50 }))
	{
		*main_menu = false;
	}

	ImGui::End();
}
