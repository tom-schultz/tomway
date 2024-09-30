#include "audio/channel.h"

bool tomway::channel::operator<(channel const& rhs) const
{
    return _impl < rhs._impl;
}

tomway::channel::operator bool() const
{
    return _impl != static_cast<SoLoud::handle>(-1);
}
