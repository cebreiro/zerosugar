#pragma once
#include <cstdint>
#include "zerosugar/shared/type/enum_class.h"

namespace zerosugar::bt::node
{
    ENUM_CLASS(State, int32_t,
        (Success)
        (Failure)
        (Running)
    )
}
