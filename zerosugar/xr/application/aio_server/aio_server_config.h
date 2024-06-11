#pragma once

namespace zerosugar::xr
{
    struct AIOServerConfig
    {
        int64_t workerCount = std::thread::hardware_concurrency();
    };
}
