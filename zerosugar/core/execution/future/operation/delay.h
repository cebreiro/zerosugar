#pragma once
#include <chrono>
#include "zerosugar/core/execution/future/future.h"

namespace zerosugar::execution
{
    auto Delay(std::chrono::milliseconds milliseconds) -> Future<void>;
}
