#pragma once

namespace zerosugar::xr
{
    struct AIOServerConfig
    {
        int64_t workerCount = std::thread::hardware_concurrency();
        std::string rpcServerIP = "127.0.0.1";
        uint16_t rpcServerPort = 6789;
    };
}
