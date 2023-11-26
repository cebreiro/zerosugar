#pragma once
#include <chrono>
#include <functional>
#include <stop_token>

namespace zerosugar::execution
{
    class IExecutor;

    auto Schedule(IExecutor& executor, const std::function<void()>& function,
        std::chrono::milliseconds firstDelay, std::chrono::milliseconds interval) -> std::stop_source;
}
