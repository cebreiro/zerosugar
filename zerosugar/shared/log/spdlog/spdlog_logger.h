#pragma once
#include <memory>
#include <mutex>
#include "zerosugar/shared/log/logger_interface.h"

namespace spdlog
{
    class logger;
    class async_logger;

    namespace sinks
    {
        class sink;
    }
}

namespace zerosugar
{
    class SpdLogConsoleLoggerConfig;
    class SpdLogDailyFileLoggerConfig;

    class SpdLogLogger final : public ILogger
    {
    public:
        SpdLogLogger() = delete;

        SpdLogLogger(const SpdLogConsoleLoggerConfig& console, const SpdLogDailyFileLoggerConfig& dailyFile);
        ~SpdLogLogger() override;

        void Log(LogLevel logLevel, const std::string& message, const std::source_location& location) override;
        void Flush() override;

    private:
        auto CreateUniqueLoggerName(bool async) -> std::string;

        static void InitializeSpdLog();

    private:
        std::shared_ptr<spdlog::logger> _syncLogger;
        std::shared_ptr<spdlog::async_logger> _asyncLogger;

        static std::once_flag _initFlag;
    };
}
