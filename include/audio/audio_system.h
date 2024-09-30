#pragma once

#include <map>
#include <string>
#include <vector>

#include "audio_config.h"
#include "soloud.h"
#include "audio/audio.h"
#include "audio/channel.h"

namespace tomway
{
    enum class channel_group { MUSIC, SFX };
    
    class audio_system
    {
    public:
        audio_system();
        ~audio_system();
        
        audio_system(audio_system&) = delete;
        audio_system(audio_system&&) = delete;
        audio_system& operator=(audio_system const&) = delete;
        audio_system& operator=(audio_system const&&) = delete;
        
        static void fade(channel& channel, float target, float time);
        static float get_volume(channel const& channel);
        static float get_volume(channel_group channel_group);
        static audio load_file(std::string const& path);
        static audio stream_file(std::string const& path);
        static channel play(audio const& audio, channel_group channel_group, float vol = 1.0f);
        static void stop(channel const& channel);
        static void set_volume(channel_group channel_group, float vol);
        static void set_global_volume(float global_vol);
        static audio_config get_audio_config();
        static void set_audio_config(audio_config audio_config);

        void new_frame();
    private:
        static inline void check_system_ready();
        static audio_system* _inst;

        std::map<channel, float> _channel_volumes;
        std::map<channel_group, std::vector<channel>> _group_channels;
        std::map<channel_group, float> _group_volumes;
        SoLoud::Soloud _soloud;
    };
}