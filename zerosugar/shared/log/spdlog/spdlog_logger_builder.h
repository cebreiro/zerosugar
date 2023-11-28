#pragma once
#include <optional>
#include "zerosugar/shared/log/spdlog/spdlog_logger_build_config.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    class SpdLogLogger;

    class SpdLogLoggerBuilder
    {
    public:
        SpdLogLoggerBuilder() = default;

        void SetConfig(const SpdLogConsoleLoggerConfig& config);
        void SetConfig(const SpdLogDailyFileLoggerConfig& config);

        auto ConfigureConsole() -> SpdLogConsoleLoggerConfig&;
        auto ConfigureDailyFile() -> SpdLogDailyFileLoggerConfig&;

        auto CreateLogger() const -> SharedPtrNotNull<SpdLogLogger>;

    private:
        std::optional<SpdLogConsoleLoggerConfig> _consoleConfig;
        std::optional<SpdLogDailyFileLoggerConfig> _dailyFileConfig;
    };
}
