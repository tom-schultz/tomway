// tomway.cpp : Defines the entry point for the application.

#include "tomway.h"

#include <fstream>

#include "tomway_utility.h"
#include "input_event.h"
#include "render_system.h"
#include "simulation/simulation_system.h"
#include "time_system.h"
#include "window_system.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Tracy.hpp"

#include "camera_controller.h"
#include "imgui.h"
#include "nfd.h"
#include "ui_system.h"
#include "audio/audio_system.h"
#include "input/input_system.h"

size_t constexpr GRID_SIZE = 600;

int main(int argc, char* argv[])
{
	tomway::simulation_system simulation_system;	
	tomway::cell_geometry cell_geometry_generator;
	tomway::window_system window_system(1024, 768);
	tomway::audio_system audio_system;
	
	tomway::ui_system ui_system(window_system);
	bool start = false, exit_loop = false;
	tomway::ui_system::bind_menu_callbacks([&start]() { start = true; }, [&exit_loop] { exit_loop = true; });

	tomway::render_system render_system(
		window_system,
		cell_geometry_generator);
	
	tomway::input_system input_system;
	tomway::time_system time_system(5);
	tomway::camera_controller camera_controller({0.0f, 0.0f, GRID_SIZE >= 1000.0f ? 990.0f : GRID_SIZE}, 90.0f, 0.0f);

	float delta = 0;
	uint32_t width, height;
	bool step = false;
	bool locked = true;
	bool loading = false;
	bool deser = false;
	std::string load_path;
	size_t new_grid_size = 0;

	auto music_audio = tomway::audio_system::stream_file("assets/audio/HoliznaCC0 - Cosmic Waves.mp3");
	auto music_channel = tomway::audio_system::play(music_audio, tomway::channel_group::MUSIC, 0);
	tomway::audio_system::fade(music_channel, 0.2f, 10);

	auto button_audio = tomway::audio_system::load_file("assets/audio/click5.ogg");
	auto iteration_audio = tomway::audio_system::load_file("assets/audio/bong_001.ogg");

#ifdef PERF
	bool main_menu = false;
	std::ifstream save_file;
	std::string data;
	save_file.open("test/600.json");
	save_file >> data;
	save_file.close();
	simulation_system.deserialize(data);
	cell_geometry_generator.bind_cells(simulation_system.get_current_cells());
#else
	tomway::ui_system::show_menu();
#endif
	
	while (true) {
		ZoneScopedN("SDL_main | game loop");
		ui_system.new_frame();
		auto window_events = window_system.handle_events();
		render_system.new_frame();
		window_system.get_vulkan_framebuffer_size(width, height);
		audio_system.new_frame();
		input_system.new_frame();
		input_system.process_events(window_events);
		delta = time_system.new_frame();
		simulation_system.new_frame();
		
		if (tomway::input_system::btn_just_up(tomway::input_button::SPACE)) step = true;
		if (tomway::input_system::btn_just_up(tomway::input_button::L)) locked = !locked;
		if (tomway::input_system::btn_just_up(tomway::input_button::F1)) window_system.toggle_mouse_visible();
		
		if (tomway::input_system::btn_just_down(tomway::input_button::ESCAPE))
		{
			tomway::ui_system::toggle_menu();
			window_system.set_mouse_visible(tomway::ui_system::is_menu_open());
		}

		if (loading)
		{
			bool deser_success = true;
			
			if (deser)
			{
				std::string data;
				std::ifstream save_file;
				save_file.open(load_path);

				if (save_file)
				{
					save_file >> data;
					save_file.close();
					deser_success = simulation_system.deserialize(data);
				}
				else
				{
					LOG_ERROR("Could not load file: %s", load_path.c_str());
				}
				
				deser = false;
			}
			else
			{
				simulation_system.start(new_grid_size);
			}
			
			loading = false;
			camera_controller.reset();
			tomway::ui_system::hide_loading_screen();

			if (deser_success)
			{
				auto const cells = simulation_system.get_current_cells();
				cell_geometry_generator.bind_cells(cells);
				tomway::ui_system::hide_menu();
			}
			else
			{
				tomway::ui_system::show_menu();
				window_system.set_mouse_visible(true);
			}
		}

		if (tomway::input_system::btn_just_up(tomway::input_button::F3))
		{
			load_path = tomway::get_file_location();

			if (not load_path.empty())
			{
				loading = true;
				deser = true;
				window_system.set_mouse_visible(false);
				camera_controller.reset();
				locked = true;
				tomway::ui_system::show_loading_screen();
			}
		}

		if (start)
		{
			tomway::ui_system::show_loading_screen();
			new_grid_size = GRID_SIZE;
			loading = true;
			start = false;
			locked = true;
		}
		
		if (not tomway::ui_system::is_menu_open())
		{
			if (tomway::input_system::btn_just_up(tomway::input_button::F2))
			{
				auto save_path = tomway::get_file_location();

				if (not save_path.empty())
				{
					std::ofstream save_file;
					save_file.open(save_path);

					if (save_file)
					{
						auto data = simulation_system.serialize();
						save_file << data;
						save_file.close();
					}
				}
			}
			
			if (tomway::input_system::btn_just_up(tomway::input_button::R))
			{
				tomway::audio_system::play(button_audio, tomway::channel_group::SFX, 0.2f);
				simulation_system.start(0);
				cell_geometry_generator.bind_cells(simulation_system.get_current_cells());
				tomway::ui_system::show_menu();
				locked = true;
				window_system.set_mouse_visible(true);
				camera_controller.reset();
			}
			
			if (tomway::input_system::btn_just_up(tomway::input_button::P))
			{
				tomway::ui_system::show_loading_screen();
				new_grid_size = 2500;
				loading = true;
			}
			
			if (not window_system.get_mouse_visible())
			{
				camera_controller.update(delta);
			}
		
			if ((!locked && time_system.get_new_tick()) || step)
			{
				simulation_system.step_simulation();
				auto cells = simulation_system.get_current_cells();
				cell_geometry_generator.bind_cells(cells);
				step = false;
				float iteration_vol = max(0.03f, tomway::audio_system::get_volume(music_channel) / 2);
				tomway::audio_system::play(iteration_audio, tomway::channel_group::SFX, iteration_vol);
			}
		}

		tomway::transform transform;
		// transform.model = glm::translate(transform.model, model_pos);
		transform.view = camera_controller.get_view_transform();
		transform.projection = camera_controller.get_projection_transform(width, height);
		
		ui_system.build_ui();

		if (exit_loop) break;
		
		render_system.draw_frame(transform);
		
		FrameMark;
	}

	exit(0);
}

std::string tomway::get_file_location()
{
    ZoneScoped;
	nfdchar_t *out_path = nullptr;
	nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &out_path );
	std::string ret = {};
        
	if (result == NFD_OKAY && out_path != nullptr)
	{
		ret = out_path;
	}
	else if (result == NFD_CANCEL)
	{
		LOG_INFO("User pressed cancel on file dialog.");
	}
	else
	{
		LOG_ERROR("Error: %s\n", NFD_GetError() );
	}

	free(out_path);
	return ret;
}