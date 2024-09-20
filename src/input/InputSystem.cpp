#include "input/InputSystem.h"

#include <stdexcept>

tomway::InputSystem* tomway::InputSystem::_inst = nullptr;

void tomway::InputSystem::check_system_ready()
{
    if (not _inst) throw std::runtime_error("Input system not available");
}

tomway::InputSystem::InputSystem()
{
    _inst = this;
}

tomway::InputSystem::~InputSystem()
{
    _inst = nullptr;
}

bool tomway::InputSystem::btn_down(InputButton const btn)
{
    check_system_ready();
    return _inst->_button_states.at(btn).down;
}

bool tomway::InputSystem::btn_just_down(InputButton const btn)
{
    check_system_ready();
    auto const state = _inst->_button_states.at(btn);
    return state.down and state.last_down == _inst->_tick;
}

bool tomway::InputSystem::btn_just_up(InputButton const btn)
{
    check_system_ready();
    auto const state = _inst->_button_states.at(btn);
    return not state.down and state.last_up == _inst->_tick; 
}

glm::vec2 tomway::InputSystem::get_mouse_vel()
{
    check_system_ready();
    return _inst->_mouse_vel;
}

void tomway::InputSystem::new_frame()
{
    _tick++;
}

void tomway::InputSystem::process_events(std::vector<InputEvent> const& events)
{
    _mouse_vel = {};
    
    for (auto const event : events)
    {
        if (event.type == InputEventType::BUTTON_DOWN)
        {
            _button_states[event.button].down = true;
            _button_states[event.button].last_down = _tick;
        }
        else if (event.type == InputEventType::BUTTON_UP)
        {
            _button_states[event.button].down = false;
            _button_states[event.button].last_up = _tick;
        }
        else if (event.type == InputEventType::MOUSE_MOTION)
        {
            _mouse_vel.x = event.mouse_x_vel;
            _mouse_vel.y = event.mouse_y_vel;
        }
    }
}
