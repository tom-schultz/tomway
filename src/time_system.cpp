#include "time_system.h"
#include "tomway_utility.h"

#include "imgui.h"
#include "ui_system.h"

tomway::time_system::time_system(float ticks_per_sec)
    :_last_frame_start_time(std::chrono::high_resolution_clock::now()),
    _start_time(std::chrono::high_resolution_clock::now()),
    _ticks_per_sec(ticks_per_sec)
{
}

bool tomway::time_system::get_new_tick() const
{
    return _new_tick;
}

float tomway::time_system::new_frame()
{
    auto const new_frame_time = std::chrono::high_resolution_clock::now();
    const float delta = std::chrono::duration<float>(new_frame_time - _last_frame_start_time).count();
    
    _last_frame_start_time = new_frame_time;
    _frame_accumulator += 1;
    _frame_timer += delta;

    if (_frame_timer >= 1.0f) {
        _fps = _frame_accumulator - _last_sec_frames;
        LOG_INFO("FPS: %d", _fps);
        _last_sec_frames = _frame_accumulator;
        _frame_timer = 0;
    }

    _tick_timer += delta;
    _new_tick = _tick_timer > 1.0f / _ticks_per_sec;
    
    if (_tick_timer > 1.0f / _ticks_per_sec)
    {
        _tick_timer = 0;
    }

    auto const fps_string = string_format("FPS: %zu", _fps);
    ui_system::add_debug_text(fps_string);
    return delta;
}
