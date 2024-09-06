#include "TimeSystem.h"
#include "HaglUtility.h"

#include "imgui.h"

tomway::TimeSystem::TimeSystem(float ticks_per_sec)
    :_last_frame_start_time(std::chrono::high_resolution_clock::now()),
    _start_time(std::chrono::high_resolution_clock::now()),
    _ticks_per_sec(ticks_per_sec)
{
}

float tomway::TimeSystem::get_millis() const
{
    auto const now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(now - _start_time).count();
}

bool tomway::TimeSystem::get_new_tick() const
{
    return _new_tick;
}

float tomway::TimeSystem::new_frame()
{
    auto const new_frame_time = std::chrono::high_resolution_clock::now();
    const float delta = std::chrono::duration<float>(new_frame_time - _last_frame_start_time).count();
    
    _last_frame_start_time = new_frame_time;
    _frame_accumulator += 1;
    _frame_timer += delta;

    if (_frame_timer > 1.0f) {
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
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    bool open = true;
    ImGui::Begin("FPS", &open, window_flags);
    ImGui::Text("FPS: %d", _fps);
    ImGui::End();
    return delta;
}
