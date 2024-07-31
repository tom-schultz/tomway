#pragma once

#include <cstdint>
#include <chrono>

namespace hagl
{
    class TimeSystem
    {
    public:
        TimeSystem();
        void start_time();
        float new_frame();
    private:
        uint64_t _frame_accumulator = 0;
        uint64_t _last_sec_frames = 0;
        float _frame_timer = 0;
        std::chrono::steady_clock::time_point _last_frame_start_time;
    };
}
