#pragma once
#include <cstdint>

namespace zerosugar::bt
{
    enum class State : int32_t
    {
        None = 0,
        Success = 1,
        Failure = 2,
        Running = 3,
    };
}
