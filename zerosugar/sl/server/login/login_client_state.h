#pragma once

namespace zerosugar::sl
{
    ENUM_CLASS(LoginClientState, int32_t,
        (Connected)
        (Authenticated)
        (WorldEntranced)
        (Invalid)
    )
}
