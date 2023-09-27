#pragma once
#include "zerosugar/shared/log/log_service.h"

namespace zerosugar
{
    class SpdLogServiceBuilder;

    class SpdLogService final : public LogService
    {
    public:
        explicit SpdLogService(const SpdLogServiceBuilder& builder);
        ~SpdLogService() override;

        void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) override;
        void Flush() override;

    private:
    };
}
