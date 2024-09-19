#pragma once

#include "soloud.h"

namespace tomway
{
    class AudioSystem;
    
    struct Channel
    {
        friend AudioSystem;
    public:
    private:
        operator bool() const;
        // Handle is an unsigned, and I will never have static_cast<unsigned>(-1) voices
        SoLoud::handle _impl = static_cast<SoLoud::handle>(-1);
        float _volume = 1.0f;
    };
}