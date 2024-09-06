#pragma once

#include <cstdint>
#include <chrono>

namespace tomway
{
    class TimeSystem
    {
    public:
        TimeSystem(float ticks_per_sec = 60);
        float get_millis() const;
        bool get_new_tick() const;
        float new_frame();
    private:
        uint64_t _frame_accumulator = 0;
        float _frame_timer = 0;
        std::chrono::high_resolution_clock::time_point _last_frame_start_time;
        uint64_t _last_sec_frames = 0;
        bool _new_tick = false;
        std::chrono::high_resolution_clock::time_point _start_time;
        float _tick_timer = 0;
        float _ticks_per_sec;
        uint64_t _fps = 0;
    };
}
