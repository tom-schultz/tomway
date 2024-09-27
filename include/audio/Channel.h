#pragma once

#include "soloud.h"

namespace tomway
{
    class AudioSystem;
    
    struct Channel
    {
        friend AudioSystem;
        bool operator<(Channel const& rhs) const;
    private:
        operator bool() const;
        // Handle is an unsigned, and I will never have static_cast<unsigned>(-1) voices
        SoLoud::handle _impl = static_cast<SoLoud::handle>(-1);
    };
}