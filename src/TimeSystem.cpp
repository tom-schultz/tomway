#include "TimeSystem.h"
#include "HaglUtility.h"

tomway::TimeSystem::TimeSystem(float ticks_per_sec)
    :_ticks_per_sec(ticks_per_sec)
{
    _last_frame_start_time = std::chrono::high_resolution_clock::now();
}

float tomway::TimeSystem::new_frame()
{
    auto const new_frame_time = std::chrono::high_resolution_clock::now();
    const float delta = std::chrono::duration<float>(new_frame_time - _last_frame_start_time).count();
    
    _last_frame_start_time = new_frame_time;
    _frame_accumulator += 1;
    _frame_timer += delta;

    if (_frame_timer > 1.0f) {
        LOG_INFO("FPS: %d", _frame_accumulator - _last_sec_frames);
        _last_sec_frames = _frame_accumulator;
        _frame_timer = 0;
    }

    _tick_timer += delta;
    _new_tick = _tick_timer > 1.0f / _ticks_per_sec;
    
    if (_tick_timer > 1.0f / _ticks_per_sec)
    {
        _tick_timer = 0;
    }

    return delta;
}

bool tomway::TimeSystem::get_new_tick() const
{
    return _new_tick;
}
