#pragma once
#include <functional>
#include <string>

#include "window_system.h"
#include "audio/audio.h"
#include "audio/audio_config.h"

namespace tomway
{
    struct sim_config
    {
        float grid_size;
        float tick_per_second;
    };
    
    using get_audio_config_fn = audio_config(*)();
    using set_audio_config_fn = void(*)(const audio_config&);
    using get_sim_config_fn = sim_config(*)();
    using set_sim_config_fn = void(*)(const sim_config&);
    using menu_start_callback = std::function<void()>;
    using menu_exit_callback = std::function<void()>;

    class ui_system
    {
    public:
        ui_system(window_system& window_system);
        static void bind_audio_config(get_audio_config_fn get_config_fn, set_audio_config_fn set_config_fn);
        
        static void bind_menu_callbacks(
            menu_start_callback const& menu_start_callback,
            menu_exit_callback const& menu_exit_callback);
        
        static void bind_sim_config(get_sim_config_fn get_config_fn, set_sim_config_fn set_config_fn);
        static void add_debug_text(std::string const& text);
        static void hide_loading_screen();
        static void hide_menu();
        static void show_loading_screen();
        static void show_menu();
        static void toggle_menu();

        void build_ui();
        bool is_menu_open() const;
        void new_frame() const;

    private:
        enum class menu_state { main_menu, audio, sim };
        static ui_system* _inst;
        
        get_audio_config_fn _audio_config_get_fn;
        set_audio_config_fn _audio_config_set_fn;
        audio _button_audio;
        menu_start_callback _menu_start_callback;
        menu_exit_callback _menu_exit_callback;
        get_sim_config_fn _sim_config_get_fn;
        set_sim_config_fn _sim_config_set_fn;
        std::vector<std::string> _debug_texts;
        
        bool _menu_open = false;
        menu_state _menu_state = menu_state::main_menu;

        void _draw_debug();
        void _draw_menu();
        void _draw_audio_menu();
        void _draw_main_menu();
    };
}


