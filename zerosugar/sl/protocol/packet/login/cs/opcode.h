#pragma once
#include <cstdint>
#include "zerosugar/shared/type/enum_class.h"

namespace zerosugar::sl::login::cs
{
    ENUM_CLASS(Opcode, int8_t,
        (LoginRequest, 0)
        (LoginBypassRequest, 1)
        (WorldSelectRequest, 2)
        (LogoutRequest, 3)
        (UnknownOpcode04, 4)
        (UnknownOpcode05, 5)
        (ReloginRequest, 6)
        (UnknownOpcode07, 7)
    )
}
