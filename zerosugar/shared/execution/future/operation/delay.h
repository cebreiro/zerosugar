#pragma once
#include <chrono>
#include "zerosugar/shared/execution/future/future.h"

namespace zerosugar::execution
{
    auto Delay(std::chrono::milliseconds milliseconds) -> Future<void>;
}
