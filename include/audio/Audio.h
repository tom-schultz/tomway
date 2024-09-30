#pragma once

#include <memory>
#include "soloud.h"

namespace tomway
{
    class audio_system;
    
    class audio
    {
        friend audio_system;
    public:
    private:
        std::shared_ptr<SoLoud::AudioSource> _impl;
    };
}