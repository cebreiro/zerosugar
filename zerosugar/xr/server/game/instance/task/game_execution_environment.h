#pragma once

namespace zerosugar::xr
{
    class GameExecutionEnvironment
    {
    public:
        GameExecutionEnvironment() = delete;

        static auto GetCurrentTimePoint() -> std::chrono::system_clock::time_point;
    };
}
