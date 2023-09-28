#pragma once
#include <cstdint>

namespace zerosugar::execution
{
    enum class FutureStatus : int32_t
    {
        Pending,
        Complete,
        Canceled,
    };
}
