#include "ui_system.h"

#include "HaglUtility.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_vulkan.h"
#include "Tracy.hpp"
#include "WindowSystem.h"
#include "audio/AudioSystem.h"

tomway::ui_system* tomway::ui_system::_inst = nullptr;

tomway::ui_system::ui_system(WindowSystem& window_system)
{
    _inst = this;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	
    auto result = window_system.init_ui_sdl(&ImGui_ImplSDL2_InitForVulkan, &ImGui_ImplSDL2_ProcessEvent);
    
    if (!result)
    {
        LOG_ERROR("ImGui failed to initialize ImGui backend!");
    }

    LOG_INFO("Initialized ImGui!");

    _button_audio = AudioSystem::load_file("assets/audio/click5.ogg");
}

void tomway::ui_system::bind_audio_config(get_audio_config_fn const get_config_fn, set_audio_config_fn const set_config_fn)
{
    _inst->_audio_config_get_fn = get_config_fn;
    _inst->_audio_config_set_fn = set_config_fn;
}

void tomway::ui_system::bind_menu_callbacks(
    menu_start_callback const& menu_start_callback,
    menu_exit_callback const& menu_exit_callback)
{
    _inst->_menu_start_callback = menu_start_callback;
    _inst->_menu_exit_callback = menu_exit_callback;
}

void tomway::ui_system::bind_sim_config(get_sim_config_fn const get_config_fn, set_sim_config_fn const set_config_fn)
{
    _inst->_sim_config_get_fn = get_config_fn;
    _inst->_sim_config_set_fn = set_config_fn;
}

void tomway::ui_system::add_debug_text(std::string const& text)
{
    _inst->_debug_texts.push_back(text);
}

void tomway::ui_system::hide_loading_screen()
{
    LOG_INFO("Hide Loading Screen");
}

void tomway::ui_system::hide_menu()
{
    _inst->_menu_open = false;
}

void tomway::ui_system::show_loading_screen()
{
    LOG_INFO("Show Loading Screen");
}

void tomway::ui_system::show_menu()
{
    _inst->_menu_open = true;
    _inst->_menu_state = menu_state::main_menu;
}

void tomway::ui_system::toggle_menu()
{
    _inst->_menu_open = not _inst->_menu_open;
    _inst->_menu_state = menu_state::main_menu;
}

void tomway::ui_system::_draw_debug()
{
    ImGuiWindowFlags constexpr window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    ImGui::Begin("Debug", nullptr, window_flags);

    for (auto const& text : _debug_texts)
    {
        ImGui::Text(text.c_str());
    }
    
    ImGui::End();

    _debug_texts.clear();
}

void tomway::ui_system::build_ui()
{
    _draw_debug();
    _draw_menu();
}

bool tomway::ui_system::is_menu_open() const
{
    return _menu_open;
}

// Do not make me static, this should only be called through the instance itself
void tomway::ui_system::new_frame() const
{
    ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void tomway::ui_system::_draw_menu()
{
    ZoneScoped;
    
    if (not _menu_open) return;

    switch (_menu_state)
    {
    case menu_state::main_menu:
        _draw_main_menu();
        break;
    case menu_state::audio:
        _draw_audio_menu();
        break;
    }
}

void tomway::ui_system::_draw_audio_menu()
{
    ImGuiWindowFlags constexpr window_flags =
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Audio Menu", nullptr, window_flags);

    auto audio_config = AudioSystem::get_audio_config();
    ImGui::SliderFloat("Global Volume", &audio_config.global_volume, 0.0f, 1.0f);
    ImGui::SliderFloat("Music Volume", &audio_config.music_volume, 0.0f, 1.0f);
    ImGui::SliderFloat("SFX Volume", &audio_config.sfx_volume, 0.0f, 1.0f);
    AudioSystem::set_audio_config(audio_config);
	
    if (ImGui::Button("Back", { 200, 50 }))
    {
        AudioSystem::play(_button_audio, ChannelGroup::SFX, 0.2f);
        _menu_state = menu_state::main_menu;
    }

    ImGui::End();
}

void tomway::ui_system::_draw_main_menu()
{
    ImGuiWindowFlags constexpr window_flags =
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav
        | ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Main Menu", nullptr, window_flags);
	
    if (ImGui::Button("Start", { 200, 50 }))
    {
        AudioSystem::play(_button_audio, ChannelGroup::SFX, 0.2f);
        _menu_open = false;
        if (_menu_start_callback) _menu_start_callback();
    }
	
    if (ImGui::Button("Audio Settings", { 200, 50 }))
    {
        AudioSystem::play(_button_audio, ChannelGroup::SFX, 0.2f);
        _menu_state = menu_state::audio;
    }
	
    if (ImGui::Button("Exit", { 200, 50 }))
    {
        if (_menu_exit_callback) _menu_exit_callback();
    }

    ImGui::End();
}
