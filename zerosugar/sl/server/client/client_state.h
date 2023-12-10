#pragma once

namespace zerosugar::sl
{
    ENUM_CLASS(LoginServerClientState, int32_t,
        (Connected)
        (Authenticated)
        (WorldEntranced)
        (Invalid)
    )
}
