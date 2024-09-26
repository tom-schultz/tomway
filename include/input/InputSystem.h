#pragma once

#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>

#include "InputEvent.h"

namespace tomway
{
    struct InputButtonState
    {
        size_t last_down = (std::numeric_limits<size_t>::max)();
        size_t last_up = (std::numeric_limits<size_t>::max)();
        bool down = false;
    };
    
    class InputSystem
    {
    public:
        InputSystem();
        ~InputSystem();
        
        InputSystem(InputSystem&) = delete;
        InputSystem(InputSystem&&) = delete;
        InputSystem& operator=(InputSystem const&) = delete;
        InputSystem& operator=(InputSystem const&&) = delete;
        
        static bool btn_down(InputButton btn);
        static bool btn_just_down(InputButton btn);
        static bool btn_just_up(InputButton btn);
        static glm::vec2 get_mouse_vel();
        void new_frame();
        void process_events(std::vector<InputEvent> const& events);
    private:
        static InputSystem* _inst;
        static inline void check_system_ready();
        glm::vec2 _mouse_vel = {};
        size_t _tick = 0;
        
        std::unordered_map<InputButton, InputButtonState> _button_states {
            { InputButton::A, {}},
            { InputButton::D, {}},
            { InputButton::L, {}},
            { InputButton::P, {}},
            { InputButton::R, {}},
            { InputButton::S, {}},
            { InputButton::W, {}},
            { InputButton::ESCAPE, {}},
            { InputButton::MOUSE_LEFT, {}},
            { InputButton::MOUSE_RIGHT, {}},
            { InputButton::SPACE, {}},
            { InputButton::F1, {}},
            { InputButton::F2, {}},
            { InputButton::F3, {}},
        };
    };
}