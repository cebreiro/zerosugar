#pragma once
#include <string>
#include <source_location>
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/log/log_level.h"

namespace zerosugar
{
    class LogService : public IService
    {
    public:
        virtual void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) = 0;
        virtual void Flush() = 0;
    };
}
