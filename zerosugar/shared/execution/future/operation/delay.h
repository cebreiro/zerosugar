#pragma once
#include <chrono>
#include "zerosugar/shared/execution/future/future.h"

namespace zerosugar
{
    auto Delay(std::chrono::milliseconds milliseconds) -> Future<void>;
}
