#pragma once
#include <cstdint>

namespace zerosugar::execution
{
    enum class ChannelSignal : int32_t
    {
        None = 0,
        NotifyOne,
        NotifyAll,
    };
}
