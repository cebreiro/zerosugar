#pragma once
#include <memory>
#include <vector>
#include "zerosugar/shared/log/log_service.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    class LogServiceComposite final : public LogService
    {
    public:
        void Add(SharedPtrNotNull<LogService> logService);

        void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) override;
        void Flush() override;

    private:
        std::vector<SharedPtrNotNull<LogService>> _services;
    };
}
