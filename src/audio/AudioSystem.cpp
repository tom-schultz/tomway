#include "audio/AudioSystem.h"

#include <memory>
#include <stdexcept>

#include "soloud_wav.h"
#include "soloud_wavstream.h"

tomway::AudioSystem* tomway::AudioSystem::_inst = nullptr;

tomway::AudioSystem::AudioSystem()
{
    _soloud.init();
    _inst = this;
}

tomway::AudioSystem::~AudioSystem()
{
    _inst = nullptr;
    _soloud.deinit();
}

tomway::Audio tomway::AudioSystem::load_file(std::string const& path)
{
    check_system_ready();
    
    auto const wav = std::make_shared<SoLoud::Wav>();
    wav->load(path.c_str());
    Audio audio;
    audio._impl = wav;
    return audio;
}

tomway::Audio tomway::AudioSystem::stream_file(std::string const& path)
{
    check_system_ready();
    
    auto const wav_stream = std::make_shared<SoLoud::WavStream>();
    wav_stream->load(path.c_str());
    Audio audio;
    audio._impl = wav_stream;
    return audio;
}

tomway::Channel tomway::AudioSystem::play(Audio const& audio, ChannelGroup channel_group, float vol)
{
    check_system_ready();
    Channel channel;

    if (_inst->_group_channels.count(channel_group) == 0)
    {
        _inst->_group_channels[channel_group] = {};
        _inst->_group_volumes[channel_group] = 1.0f;
    }

    _inst->_group_channels[channel_group].push_back(channel);
    float const group_vol = _inst->_group_volumes[channel_group];
    float const global_vol = _inst->_soloud.getGlobalVolume();
    channel._impl = _inst->_soloud.play(*audio._impl, vol * group_vol * global_vol);
    channel._volume = vol;
    return channel;
}

void tomway::AudioSystem::stop(Channel const& channel)
{
    check_system_ready();
    if (not channel) return;
    
    _inst->_soloud.stop(channel._impl);
}

void tomway::AudioSystem::set_volume(ChannelGroup channel_group, float group_vol)
{
    check_system_ready();
    if (_inst->_group_channels.count(channel_group) == 0) return;

    float const global_vol = _inst->_soloud.getGlobalVolume();

    for(auto const& channel : _inst->_group_channels[channel_group])
    {
        float const channel_vol = _inst->_soloud.getVolume(channel._impl);
        _inst->_soloud.setVolume(channel, channel_vol * group_vol * global_vol);
    }

    _inst->_group_volumes[channel_group] = group_vol;
}

void tomway::AudioSystem::set_global_volume(float global_vol)
{
    check_system_ready();
    _inst->_soloud.setGlobalVolume(global_vol);
    float group_vol = 0.0f;

    for(auto const& pair : _inst->_group_channels)
    {
        group_vol = _inst->_group_volumes[pair.first];
        
        for (auto const& channel : pair.second) {
            _inst->_soloud.setVolume(channel._impl, channel._volume * group_vol * global_vol);
        }
    }
}

void tomway::AudioSystem::fade(Channel const& channel, float target, float time)
{
    check_system_ready();
    if (not channel) return;
    
    _inst->_soloud.fadeVolume(channel._impl, target, time);
}

float tomway::AudioSystem::get_volume(Channel const& channel)
{
    check_system_ready();
    return channel._volume;
}

float tomway::AudioSystem::get_volume(ChannelGroup channel_group)
{
    return _inst->_group_volumes[channel_group];
}

void tomway::AudioSystem::check_system_ready()
{
    if (not _inst) throw std::runtime_error("Audio system not available");
}
