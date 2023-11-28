#pragma once
#include <source_location>
#include "zerosugar/shared/log/log_level.h"
#include "spdlog/spdlog.h"

namespace zerosugar::detail
{
    auto ToSpdLog(LogLevel level) -> spdlog::level::level_enum;
    auto ToSpdLog(const std::source_location& location) -> spdlog::source_loc;
}
