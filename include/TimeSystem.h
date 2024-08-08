#pragma once

#include <cstdint>
#include <chrono>

namespace tomway
{
    class TimeSystem
    {
    public:
        TimeSystem(float ticks_per_sec = 60);
        float new_frame();
        bool get_new_tick() const;
    private:
        uint64_t _frame_accumulator = 0;
        uint64_t _last_sec_frames = 0;
        float _frame_timer = 0;
        std::chrono::steady_clock::time_point _last_frame_start_time;
        bool _new_tick = false;
        float _tick_timer = 0;
        float _ticks_per_sec;
    };
}
