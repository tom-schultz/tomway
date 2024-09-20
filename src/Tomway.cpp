// tomway.cpp : Defines the entry point for the application.

#include "Tomway.h"

#include <fstream>

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

#include "CameraController.h"
#include "imgui.h"
#include "nfd.h"
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
	tomway::TimeSystem time_system(5);
	tomway::CameraController camera_controller({0.0f, 0.0f, GRID_SIZE}, 90.0f, 0.0f);
	
	tomway::RenderSystem render_system(
		window_system,
		cell_geometry_generator,
		tomway::CellGeometry::max_vertex_count(GRID_SIZE * GRID_SIZE));

	float delta = 0;
	uint32_t width, height;
	bool step = false;
	bool locked = true;
	bool main_menu = true;

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
			camera_controller.reset();
		}

		if (tomway::InputSystem::btn_just_up(tomway::InputButton::F2))
		{
			auto data = simulation_system.serialize();
			auto save_path = tomway::get_save_location();

			std::ofstream save_file;
			save_file.open(save_path);
			save_file << data;
			save_file.close();
		}

		if (main_menu)
		{
			tomway::draw_main_menu(&main_menu);
			
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
				camera_controller.update(delta);
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
		// transform.model = glm::translate(transform.model, model_pos);
		transform.view = camera_controller.get_view_transform();
		transform.projection = camera_controller.get_projection_transform(width, height);
		
		auto cells = simulation_system.get_current_cells();
		cell_geometry_generator.bind_cells(cells);
		render_system.draw_frame(transform);
	}

	exit(0);
}

void tomway::draw_main_menu(bool* main_menu)
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

std::string tomway::get_save_location()
{
	nfdchar_t *outPath = NULL;
	nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
        
	if ( result == NFD_OKAY ) {
		LOG_INFO(outPath);
	}
	else if ( result == NFD_CANCEL ) {
		LOG_INFO("User pressed cancel.");
	}
	else {
		LOG_ERROR("Error: %s\n", NFD_GetError() );
	}

	std::string ret(outPath);
	free(outPath);
	return ret;
}
