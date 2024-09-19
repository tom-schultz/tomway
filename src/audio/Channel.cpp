#include "audio/Channel.h"


tomway::Channel::operator bool() const
{
    return _impl != static_cast<SoLoud::handle>(-1);
}