#pragma once
#include <cstdint>

namespace zerosugar
{
    enum class FutureStatus : int32_t
    {
        Pending,
        Complete,
        Canceled,
    };
}
