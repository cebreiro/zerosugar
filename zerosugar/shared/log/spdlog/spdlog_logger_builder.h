#pragma once
#include <optional>
#include <mutex>
#include "zerosugar/shared/log/spdlog/spdlog_logger_build_config.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar
{
    class SpdLogLogger;

    class SpdLogLoggerBuilder
    {
    public:
        SpdLogLoggerBuilder();

        void SetConfig(const SpdLogConsoleLoggerConfig& config);
        void SetConfig(const SpdLogDailyFileLoggerConfig& config);

        auto ConfigureConsole() -> SpdLogConsoleLoggerConfig&;
        auto ConfigureDailyFile() -> SpdLogDailyFileLoggerConfig&;

        auto CreateLogger() const -> SharedPtrNotNull<SpdLogLogger>;

    private:
        static void InitializeSpdLog();

    private:
        std::optional<SpdLogConsoleLoggerConfig> _consoleConfig;
        std::optional<SpdLogDailyFileLoggerConfig> _dailyFileConfig;

        static std::once_flag _initFlag;
    };
}
