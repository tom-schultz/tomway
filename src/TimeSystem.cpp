#include "TimeSystem.h"
#include "HaglUtility.h"

hagl::TimeSystem::TimeSystem()
{
    _last_frame_start_time = std::chrono::high_resolution_clock::now();
}

float hagl::TimeSystem::new_frame()
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

    return delta;
}
