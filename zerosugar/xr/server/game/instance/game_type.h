#pragma once

namespace zerosugar::xr
{
    using game_clock_type = std::chrono::system_clock;
    using game_time_point_type = game_clock_type::time_point;

    ENUM_CLASS(ChattingType, int32_t,
        (System)
        (Local)
        (Global)
    )
}

namespace zerosugar::xr
{
    auto GetMilliFromGameSeconds(double seconds) -> std::chrono::milliseconds;
    auto GetSpeedFromClientValue(float speed) -> float;
}
