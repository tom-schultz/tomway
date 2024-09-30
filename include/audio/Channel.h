#pragma once

#include "soloud.h"

namespace tomway
{
    class audio_system;
    
    struct channel
    {
        friend audio_system;
        bool operator<(channel const& rhs) const;
    private:
        operator bool() const;
        // Handle is an unsigned, and I will never have static_cast<unsigned>(-1) voices
        SoLoud::handle _impl = static_cast<SoLoud::handle>(-1);
    };
}