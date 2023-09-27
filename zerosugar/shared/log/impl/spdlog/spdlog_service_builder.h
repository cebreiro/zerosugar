#pragma once
#include <string>
#include <optional>
#include "zerosugar/shared/log/impl/spdlog/spdlog_serivce_build_config.h"

namespace zerosugar
{
    class SpdLogServiceBuilder
    {
        friend class SpdLogService;

    public:
        SpdLogServiceBuilder() = default;

        void SetConsoleConfig(SpdLogConsoleLogConfig config);
        void SetDailyFileLogConfig(SpdLogDailyFileLogConfig config);

        auto ConfigureConsoleLog() -> SpdLogConsoleLogConfig&;
        auto ConfigureDailyFileLog() -> SpdLogDailyFileLogConfig&;

    private:
        std::optional<SpdLogConsoleLogConfig> _consoleLogConfig;
        std::optional<SpdLogDailyFileLogConfig> _dailyFileLogConfig;
    };
}
