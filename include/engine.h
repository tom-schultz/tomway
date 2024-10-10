#pragma once
#include "camera_controller.h"
#include "time_system.h"
#include "ui_system.h"
#include "window_system.h"
#include "audio/audio_system.h"
#include "input/input_system.h"
#include "render/cell_geometry.h"
#include "render/render_system.h"
#include "simulation/simulation_system.h"

namespace tomway
{
    class engine
    {
    public:
        engine(size_t const& grid_size);
        ~engine() = default;
        engine(engine&) = delete;
        engine(engine&&) = delete;
        engine& operator=(engine const&) = delete;
        engine& operator=(engine const&&) = delete;

        void run();
    private:
        static std::string _get_file_location();

        void _load_file();
        void _load_perf();
        void _load_sim();
        void _reset_sim();
        void _save_sim() const;
        void _start_sim();
        void _step_sim();
        
        simulation_system _simulation_system;	
        cell_geometry _cell_geometry_generator;
        window_system _window_system = { 1024, 768 };
        audio_system _audio_system;
	    ui_system _ui_system { _window_system };
        render_system _render_system { _window_system, _cell_geometry_generator };
        input_system _input_system;
        time_system _time_system { 5 };
	    camera_controller _camera_controller;

        float _delta = 0;
        uint32_t _width = 1024, _height = 768;
        size_t _grid_size;
        bool _step = false;
        bool _exit_loop = false;
        bool _locked = true;
        bool _loading = false;
        bool _deser = false;
        bool _start = false;
        std::string _load_path;
        size_t _new_grid_size = 0;
        audio _button_audio;
        audio _iteration_audio;
        audio _music_audio;
        channel _music_channel;
    };
}
