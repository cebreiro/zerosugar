#pragma once

namespace zerosugar::xr::coordination
{
    struct ServerStatus
    {
        int32_t loadCPUPercentage = std::numeric_limits<int32_t>::min();
        double freePhysicalMemoryGB = std::numeric_limits<double>::min();
    };
}
