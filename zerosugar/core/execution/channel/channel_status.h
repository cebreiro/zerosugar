#pragma once
#include <cstdint>

namespace zerosugar::execution
{
    enum class ChannelStatus : int32_t
    {
        Open,
        Closed,
    };
}
