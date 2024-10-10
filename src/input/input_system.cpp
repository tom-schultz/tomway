#include "input/input_system.h"

#include <stdexcept>

#include "tomway_utility.h"
#include "Tracy.hpp"

tomway::input_system* tomway::input_system::_inst = nullptr;

void tomway::input_system::check_system_ready()
{
    if (not _inst) throw std::runtime_error("Input system not available");
}

tomway::input_system::input_system()
{
    _inst = this;
}

tomway::input_system::~input_system()
{
    _inst = nullptr;
}

bool tomway::input_system::btn_down(input_button const btn)
{
    check_system_ready();
    return _inst->_button_states.at(btn).down;
}

bool tomway::input_system::btn_just_down(input_button const btn)
{
    ZoneScoped;
    check_system_ready();
    auto const state = _inst->_button_states.at(btn);
    return state.down and state.last_down == _inst->_tick;
}

bool tomway::input_system::btn_just_up(input_button const btn)
{
    ZoneScoped;
    check_system_ready();
    auto const state = _inst->_button_states.at(btn);
    return not state.down and state.last_up == _inst->_tick; 
}

glm::vec2 tomway::input_system::get_mouse_delta()
{
    check_system_ready();
    return _inst->_mouse_vel;
}

void tomway::input_system::new_frame()
{
    _tick++;
}

void tomway::input_system::process_events(std::vector<input_event> const& events)
{
    ZoneScoped;
    _mouse_vel = {};
    size_t mouse_motion_events = 0;
    
    for (auto const event : events)
    {
        if (event.type == input_event_type::BUTTON_DOWN)
        {
            _button_states[event.button].down = true;
            _button_states[event.button].last_down = _tick;
        }
        else if (event.type == input_event_type::BUTTON_UP)
        {
            _button_states[event.button].down = false;
            _button_states[event.button].last_up = _tick;
        }
        else if (event.type == input_event_type::MOUSE_MOTION)
        {
            _mouse_vel.x += event.mouse_x_vel;
            _mouse_vel.y += event.mouse_y_vel;
            mouse_motion_events++;
        }
    }

    if (mouse_motion_events > 1)
    {
        LOG_INFO("Mouse motion events: %zu", mouse_motion_events);
    }
}
