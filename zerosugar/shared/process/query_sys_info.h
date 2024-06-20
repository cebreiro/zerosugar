#pragma once
#include <cstdint>

namespace zerosugar
{
    auto QueryCPULoadPercentage() -> int32_t;
    auto QueryFreePhysicalMemoryGB() -> double;
}
