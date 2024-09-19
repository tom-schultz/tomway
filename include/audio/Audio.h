#pragma once

#include <memory>
#include "soloud.h"

namespace tomway
{
    class AudioSystem;
    
    class Audio
    {
        friend AudioSystem;
    public:
    private:
        std::shared_ptr<SoLoud::AudioSource> _impl;
    };
}