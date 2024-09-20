#pragma once

#include <string>
#include <unordered_map>

#include "soloud.h"
#include "audio/Audio.h"
#include "audio/Channel.h"

namespace tomway
{
    enum class ChannelGroup { MUSIC, SFX };
    
    class AudioSystem
    {
    public:
        AudioSystem();
        ~AudioSystem();
        
        AudioSystem(AudioSystem&) = delete;
        AudioSystem(AudioSystem&&) = delete;
        AudioSystem& operator=(AudioSystem const&) = delete;
        AudioSystem& operator=(AudioSystem const&&) = delete;
        
        static void fade(Channel const& channel, float target, float time);
        static float get_volume(Channel const& channel);
        static float get_volume(ChannelGroup channel_group);
        static Audio load_file(std::string const& path);
        static Audio stream_file(std::string const& path);
        static Channel play(Audio const& audio, ChannelGroup channel_group, float vol = 1.0f);
        static void stop(Channel const& channel);
        static void set_volume(ChannelGroup channel_group, float vol);
        static void set_global_volume(float global_vol);

        void new_frame();
    private:
        static inline void check_system_ready();
        static AudioSystem* _inst;

        std::unordered_map<ChannelGroup, std::vector<Channel>> _group_channels;
        std::unordered_map<ChannelGroup, float> _group_volumes;
        SoLoud::Soloud _soloud;
    };
}