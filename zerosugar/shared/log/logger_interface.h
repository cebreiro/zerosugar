#pragma once
#include <string>
#include <source_location>
#include "zerosugar/shared/log/log_level.h"

namespace zerosugar
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        virtual void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) = 0;
        virtual void Flush() = 0;
    };
}
