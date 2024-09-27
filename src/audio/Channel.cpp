#include "audio/Channel.h"

bool tomway::Channel::operator<(Channel const& rhs) const
{
    return _impl < rhs._impl;
}

tomway::Channel::operator bool() const
{
    return _impl != static_cast<SoLoud::handle>(-1);
}
