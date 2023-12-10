#pragma once
#include <cstdint>
#include "zerosugar/shared/type/enum_class.h"

namespace zerosugar::sl::login::sc
{
    ENUM_CLASS(Opcode, int8_t,
        (Hello, 0)
        (LoginFail, 1)
        (WorldList, 4)
        (WorldEnter, 7)
    )
}
