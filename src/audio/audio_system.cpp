#include "audio/audio_system.h"

#include <memory>
#include <stdexcept>

#include "soloud_wav.h"
#include "soloud_wavstream.h"
#include "Tracy.hpp"

tomway::audio_system* tomway::audio_system::_inst = nullptr;

tomway::audio_system::audio_system()
    : _group_volumes({{ channel_group::MUSIC, 1.0f}, { channel_group::SFX, 1.0f }})
{
    _soloud.init();
    _inst = this;
}

tomway::audio_system::~audio_system()
{
    _inst = nullptr;
    _soloud.deinit();
}

void tomway::audio_system::check_system_ready()
{
    if (not _inst) throw std::runtime_error("Audio system not available");
}

void tomway::audio_system::fade(channel& channel, float target, float time)
{
    check_system_ready();
    if (not channel) return;

    _inst->_channel_volumes[channel] = target;
    _inst->_soloud.fadeVolume(channel._impl, target, time);
}

float tomway::audio_system::get_volume(channel const& channel)
{
    check_system_ready();
    return _inst->_channel_volumes[channel];
}

float tomway::audio_system::get_volume(channel_group channel_group)
{
    return _inst->_group_volumes[channel_group];
}

tomway::audio tomway::audio_system::load_file(std::string const& path)
{
    check_system_ready();
    
    auto const wav = std::make_shared<SoLoud::Wav>();
    wav->load(path.c_str());
    audio audio;
    audio._impl = wav;
    return audio;
}

tomway::audio tomway::audio_system::stream_file(std::string const& path)
{
    check_system_ready();
    
    auto const wav_stream = std::make_shared<SoLoud::WavStream>();
    wav_stream->load(path.c_str());
    audio audio;
    audio._impl = wav_stream;
    return audio;
}

tomway::channel tomway::audio_system::play(audio const& audio, channel_group channel_group, float vol)
{
    ZoneScoped;
    check_system_ready();
    channel channel;

    if (_inst->_group_channels.count(channel_group) == 0)
    {
        _inst->_group_channels[channel_group] = {};
        _inst->_group_volumes[channel_group] = 1.0f;
    }
    
    float const group_vol = _inst->_group_volumes[channel_group];
    float const global_vol = _inst->_soloud.getGlobalVolume();
    channel._impl = _inst->_soloud.play(*audio._impl, vol * group_vol * global_vol);
    _inst->_channel_volumes[channel] = vol;
    _inst->_group_channels[channel_group].push_back(channel);
    
    return channel;
}

void tomway::audio_system::stop(channel const& channel)
{
    check_system_ready();
    if (not channel) return;
    
    _inst->_soloud.stop(channel._impl);
}

void tomway::audio_system::set_volume(channel_group channel_group, float group_vol)
{
    check_system_ready();
    if (_inst->_group_channels.count(channel_group) == 0) return;

    float const global_vol = _inst->_soloud.getGlobalVolume();

    for(auto const& channel : _inst->_group_channels[channel_group])
    {
        if (_inst->_soloud.isFading(channel)) continue;
        auto channel_vol = _inst->_channel_volumes[channel];
        _inst->_soloud.setVolume(channel, channel_vol * group_vol * global_vol);
    }

    _inst->_group_volumes[channel_group] = group_vol;
}

void tomway::audio_system::set_global_volume(float global_vol)
{
    check_system_ready();
    _inst->_soloud.setGlobalVolume(global_vol);

    for(auto const& pair : _inst->_group_channels)
    {
        float group_vol = _inst->_group_volumes[pair.first];
        
        for (auto const& channel : pair.second) {
            if (not _inst->_soloud.isValidVoiceHandle(channel._impl)) continue;
            auto channel_vol = _inst->_channel_volumes[channel];
            _inst->_soloud.setVolume(channel._impl, channel_vol * group_vol * global_vol);
        }
    }
}

tomway::audio_config tomway::audio_system::get_audio_config()
{
    return {
        _inst->_soloud.getGlobalVolume(),
        _inst->_group_volumes[channel_group::MUSIC],
        _inst->_group_volumes[channel_group::SFX]
    };
}

void tomway::audio_system::set_audio_config(audio_config audio_config)
{
    _inst->_soloud.setGlobalVolume(audio_config.global_volume);
    set_volume(channel_group::MUSIC, audio_config.music_volume);
    set_volume(channel_group::SFX, audio_config.sfx_volume);
}

void tomway::audio_system::new_frame()
{
    check_system_ready();
    
    for(auto& pair : _inst->_group_channels)
    {
        auto& channels = pair.second;
        
        for (size_t i = 0; i < channels.size(); i++) {
            if (_soloud.isValidVoiceHandle(channels[i]._impl)) continue;
            
            channels.erase(channels.begin() + i);
            i--;
        }
    }
}
