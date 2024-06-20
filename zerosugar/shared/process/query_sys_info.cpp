#include "query_sys_info.h"

#include <cassert>
#include <charconv>
#include "zerosugar/shared/process/execute_command.h"

namespace zerosugar
{
    auto QueryCPULoadPercentage() -> int32_t
    {
        std::vector<std::string> results = ExecuteCommand("wmic cpu get LoadPercentage");
        assert(std::ssize(results) == 2);

        const std::string& str = results.at(1);

        int32_t converted = 0;
        const std::errc ec = std::from_chars(str.data(), str.data() + str.size(), converted).ec;
        assert(ec == std::errc());

        return converted;
    }

    auto QueryFreePhysicalMemoryGB() -> double
    {
        std::vector<std::string> results = ExecuteCommand("OS get FreePhysicalMemory");
        assert(std::ssize(results) == 2);

        const std::string& str = results.at(1);

        double conv = 0;
        const std::errc ec = std::from_chars(str.data(), str.data() + str.size(), conv).ec;
        assert(ec == std::errc());

        return conv / (1024 * 1024);
    }
}
