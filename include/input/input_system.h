#pragma once

#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>

#include "input_event.h"

namespace tomway
{
    struct InputButtonState
    {
        size_t last_down = (std::numeric_limits<size_t>::max)();
        size_t last_up = (std::numeric_limits<size_t>::max)();
        bool down = false;
    };
    
    class input_system
    {
    public:
        input_system();
        ~input_system();
        input_system(input_system&) = delete;
        input_system(input_system&&) = delete;
        input_system& operator=(input_system const&) = delete;
        input_system& operator=(input_system const&&) = delete;
        
        static bool btn_down(input_button btn);
        static bool btn_just_down(input_button btn);
        static bool btn_just_up(input_button btn);
        static glm::vec2 get_mouse_delta();
        void new_frame();
        void process_events(std::vector<input_event> const& events);
    private:
        static input_system* _inst;
        static inline void check_system_ready();
        glm::vec2 _mouse_vel = {};
        size_t _tick = 0;
        
        std::unordered_map<input_button, InputButtonState> _button_states {
            { input_button::A, {}},
            { input_button::D, {}},
            { input_button::L, {}},
            { input_button::P, {}},
            { input_button::R, {}},
            { input_button::S, {}},
            { input_button::W, {}},
            { input_button::ESCAPE, {}},
            { input_button::MOUSE_LEFT, {}},
            { input_button::MOUSE_RIGHT, {}},
            { input_button::SPACE, {}},
            { input_button::F1, {}},
            { input_button::F2, {}},
            { input_button::F3, {}},
        };
    };
}