#pragma once

namespace zerosugar::sl
{
    ENUM_CLASS(GatewayClientState, int32_t,
        (Connected)
        (VersionChecked)
        (Authenticated)
        (ZoneEntered)
    )
}
