#include "engine.h"

#include <fstream>

#include "nfd.h"
#include "tomway_utility.h"
#include "Tracy.hpp"

tomway::engine::engine(size_t const& grid_size)
	: _camera_controller({0.0f, 0.0f, grid_size >= 1000.0f ? 990.0f : grid_size}, 90.0f, 0.0f),
	_grid_size(grid_size)
{
    
}

void tomway::engine::run()
{
	ui_system::bind_menu_callbacks([this] { _start = true; }, [this] { _exit_loop = true; });
	
	_music_audio = audio_system::stream_file("assets/audio/HoliznaCC0 - Cosmic Waves.mp3");
	_music_channel = audio_system::play(_music_audio, tomway::channel_group::MUSIC, 0);
	audio_system::fade(_music_channel, 0.2f, 60);
	
	_button_audio = audio_system::load_file("assets/audio/click5.ogg");
	_iteration_audio = audio_system::load_file("assets/audio/bong_001.ogg");

#ifdef PERF
	_load_perf();
#else
	ui_system::show_menu();
#endif
	while (true) {
		ZoneScopedN("SDL_main | game loop");
		_ui_system.new_frame();
		auto window_events = _window_system.handle_events();
		_render_system.new_frame();
		_window_system.get_vulkan_framebuffer_size(_width, _height);
		_audio_system.new_frame();
		_input_system.new_frame();
		_input_system.process_events(window_events);
		_delta = _time_system.new_frame();
		_simulation_system.new_frame();
		
		if (input_system::btn_just_up(input_button::SPACE)) _step = true;
		if (input_system::btn_just_up(input_button::L)) _locked = !_locked;
		if (input_system::btn_just_up(input_button::F1)) _window_system.toggle_mouse_visible();
		
		if (input_system::btn_just_down(input_button::ESCAPE))
		{
			ui_system::toggle_menu();
			_window_system.set_mouse_visible(ui_system::is_menu_open());
		}

		if (input_system::btn_just_up(input_button::F2)) _save_sim();
		// _load_sim must happen before the P handler, _load_file, and _start_sim so that the loading screen is drawn
		if (_loading) _load_sim();
		
		if (input_system::btn_just_up(input_button::P))
		{
			ui_system::show_loading_screen();
			_new_grid_size = 2500;
			_loading = true;
		}
		
		if (input_system::btn_just_up(input_button::F3)) _load_file();
		if (_start) _start_sim();
		
		if (not ui_system::is_menu_open())
		{
			if (input_system::btn_just_up(input_button::R)) _reset_sim();
			if ((!_locked && _time_system.get_new_tick()) || _step) _step_sim();
			if (not _window_system.get_mouse_visible()) _camera_controller.update(_delta);
		}

		if (_exit_loop) break;
		
		transform transform;
		transform.view = _camera_controller.get_view_transform();
		transform.projection = _camera_controller.get_projection_transform(_width, _height);
		_ui_system.build_ui();
		_render_system.draw_frame(transform);
		
		FrameMark;
	}
}

std::string tomway::engine::_get_file_location()
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

void tomway::engine::_load_file()
{
	_load_path = _get_file_location();

	if (not _load_path.empty())
	{
		_loading = true;
		_deser = true;
		_window_system.set_mouse_visible(false);
		_camera_controller.reset();
		_locked = true;
		ui_system::show_loading_screen();
	}
}

void tomway::engine::_load_perf()
{
	std::ifstream save_file;
	std::string data;
	save_file.open("test/600.json");
	save_file >> data;
	save_file.close();
	_simulation_system.deserialize(data);
	_cell_geometry_generator.bind_cells(_simulation_system.get_current_cells());
}

void tomway::engine::_load_sim()
{
	bool deser_success = true;
			
	if (_deser)
	{
		std::string data;
		std::ifstream save_file;
		save_file.open(_load_path);

		if (save_file)
		{
			save_file >> data;
			save_file.close();
			deser_success = _simulation_system.deserialize(data);
		}
		else
		{
			LOG_ERROR("Could not load file: %s", _load_path.c_str());
		}
				
		_deser = false;
	}
	else
	{
		_simulation_system.start(_new_grid_size);
	}
			
	_loading = false;
	_camera_controller.reset();
	ui_system::hide_loading_screen();

	if (deser_success)
	{
		auto const cells = _simulation_system.get_current_cells();
		_cell_geometry_generator.bind_cells(cells);
		ui_system::hide_menu();
	}
	else
	{
		ui_system::show_menu();
		_window_system.set_mouse_visible(true);
	}
}

void tomway::engine::_reset_sim()
{
	audio_system::play(_button_audio, channel_group::SFX, 0.2f);
	_simulation_system.start(0);
	_cell_geometry_generator.bind_cells(_simulation_system.get_current_cells());
	ui_system::show_menu();
	_locked = true;
	_window_system.set_mouse_visible(true);
	_camera_controller.reset();
}

void tomway::engine::_save_sim() const
{
	if (_simulation_system.get_cell_count() == 0) return;
	
	auto save_path = _get_file_location();

	if (not save_path.empty())
	{
		std::ofstream save_file;
		save_file.open(save_path);

		if (save_file)
		{
			auto data = _simulation_system.serialize();
			save_file << data;
			save_file.close();
		}
	}
}

void tomway::engine::_start_sim()
{
	ui_system::show_loading_screen();
	_new_grid_size = _grid_size;
	_loading = true;
	_start = false;
	_locked = true;
}

void tomway::engine::_step_sim()
{
	_simulation_system.step_simulation();
	auto cells = _simulation_system.get_current_cells();
	_cell_geometry_generator.bind_cells(cells);
	_step = false;
	float iteration_vol = std::max(0.03f, audio_system::get_volume(_music_channel) / 2);
	audio_system::play(_iteration_audio, channel_group::SFX, iteration_vol);
}
