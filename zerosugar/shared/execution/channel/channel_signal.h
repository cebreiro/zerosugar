#pragma once
#include <cstdint>

namespace zerosugar::channel
{
    enum class ChannelSignal : int32_t
    {
        None = 0,
        NotifyOne,
        NotifyAll,
    };
}
