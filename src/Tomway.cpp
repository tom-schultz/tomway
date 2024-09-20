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
#include "soloud.h"
#include "audio/AudioSystem.h"
#include "input/InputSystem.h"

size_t constexpr GRID_SIZE = 40;

int main(int argc, char* argv[])
{
	tomway::SimulationSystem simulation_system;	
	tomway::CellGeometry cell_geometry_generator(simulation_system.get_current_cells());
	tomway::WindowSystem window_system(1024, 768);
	tomway::AudioSystem audio_system;
	tomway::InputSystem input_system;
	
	tomway::RenderSystem render_system(
		window_system,
		cell_geometry_generator,
		tomway::CellGeometry::max_vertex_count(GRID_SIZE * GRID_SIZE));

	tomway::TimeSystem time_system(5);
	float delta = 0;
	
	uint32_t width, height;
	glm::vec3 model_pos(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_pos(0.0f, 0.0f, GRID_SIZE);
	
	bool step = false;
	bool locked = true;
	bool main_menu = true;

	glm::vec3 fwd = {0, 0, 0};
	float vert_rot = 90.0f;
	float hor_rot = 0.01f;

	auto music_audio = tomway::AudioSystem::stream_file("assets/audio/HoliznaCC0 - Cosmic Waves.mp3");
	auto music_channel = tomway::AudioSystem::play(music_audio, tomway::ChannelGroup::MUSIC, 0);
	tomway::AudioSystem::fade(music_channel, 0.2f, 90);

	auto button_audio = tomway::AudioSystem::load_file("assets/audio/click5.ogg");
	auto iteration_audio = tomway::AudioSystem::load_file("assets/audio/bong_001.ogg");
	
	while (true) {
		auto input_events = window_system.handle_events();
		render_system.new_frame();
		window_system.get_vulkan_framebuffer_size(width, height);
		audio_system.new_frame();
		input_system.new_frame();
		input_system.process_events(input_events);
		delta = time_system.new_frame();
		
		if (tomway::InputSystem::btn_just_down(tomway::InputButton::ESCAPE)) break;
		if (tomway::InputSystem::btn_just_up(tomway::InputButton::SPACE)) step = true;
		if (tomway::InputSystem::btn_just_up(tomway::InputButton::L)) locked = !locked;
		if (tomway::InputSystem::btn_just_up(tomway::InputButton::F1)) window_system.toggle_mouse_visible();

		if (tomway::InputSystem::btn_just_up(tomway::InputButton::R))
		{
			tomway::AudioSystem::play(button_audio, tomway::ChannelGroup::SFX, 0.2f);
			simulation_system.start(0);
			main_menu = true;
			locked = true;
			window_system.set_mouse_visible(true);
			camera_pos = { 0.0f, 0.0f, GRID_SIZE };
			hor_rot = 0;
			vert_rot = 90;
		}
		
		glm::mat4 rotation_mat(1.0f);
		rotation_mat = glm::rotate(rotation_mat, glm::radians(vert_rot), glm::vec3(1.0f, 0.0f, 0.0f));
		rotation_mat = glm::rotate(rotation_mat, glm::radians(hor_rot), glm::vec3(0.0f, 0.0f, 1.0f));
		
		glm::vec3 right = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) * rotation_mat;
		fwd = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * rotation_mat;
		glm::vec3 up = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * rotation_mat;

		if (main_menu)
		{
			draw_main_menu(&main_menu);
			
			if (not main_menu)
			{
				simulation_system.start(GRID_SIZE);
				tomway::AudioSystem::play(button_audio, tomway::ChannelGroup::SFX, 0.2f);
				window_system.set_mouse_visible(false);
			}
		}
		else
		{
			if (not window_system.get_mouse_visible())
			{
				glm::vec2 mouse_vel = tomway::InputSystem::get_mouse_vel();
				
				// Inverted
				vert_rot -= mouse_vel.y * delta * 180.0f;
				hor_rot -= mouse_vel.x * delta * 180.0f;
		
				float constexpr speed = 4.0f;
		
				if (tomway::InputSystem::btn_down(tomway::InputButton::W)) camera_pos += fwd * speed * delta;
				if (tomway::InputSystem::btn_down(tomway::InputButton::S)) camera_pos -= fwd * speed * delta;
				if (tomway::InputSystem::btn_down(tomway::InputButton::A)) camera_pos -= right * speed * delta;
				if (tomway::InputSystem::btn_down(tomway::InputButton::D)) camera_pos += right * speed * delta;
			}
		
			if ((!locked && time_system.get_new_tick()) || step)
			{
				simulation_system.step_simulation();
				step = false;
				float iteration_vol = max(0.03f, tomway::AudioSystem::get_volume(music_channel) / 2);
				tomway::AudioSystem::play(iteration_audio, tomway::ChannelGroup::SFX, iteration_vol);
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
		auto cells = simulation_system.get_current_cells();
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