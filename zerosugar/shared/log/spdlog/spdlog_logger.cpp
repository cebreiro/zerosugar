#include "spdlog_logger.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_build_config.h"

namespace zerosugar
{
    auto ToSpdLog(LogLevel level) -> spdlog::level::level_enum
    {
        switch (level)
        {
        case LogLevel::Debug: return spdlog::level::level_enum::debug;
        case LogLevel::Info: return spdlog::level::level_enum::info;
        case LogLevel::Warn: return spdlog::level::level_enum::warn;
        case LogLevel::Error: return spdlog::level::level_enum::err;
        case LogLevel::Critical: return spdlog::level::level_enum::critical;
        default:
            assert(false);
            return spdlog::level::level_enum::debug;
        }
    }

    auto ToSpdLog(const std::source_location& location) -> spdlog::source_loc
    {
        return spdlog::source_loc{
            location.file_name(),
            static_cast<int>(location.line()),
            location.function_name()
        };
    }

    SpdLogLogger::SpdLogLogger(const SpdLogConsoleLoggerConfig& console, const SpdLogDailyFileLoggerConfig& dailyFile)
    {
        InitializeSpdLog();

        std::vector<std::shared_ptr<spdlog::sinks::sink>> syncSinks;
        std::vector<std::shared_ptr<spdlog::sinks::sink>> asyncSinks;
        {
            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sink->set_level(ToSpdLog(console.GetLogLevel()));
            sink->set_pattern(console.GetPattern());

            for (const auto& [level, color] : console.GetColors())
            {
                sink->set_color(ToSpdLog(level), color);
            }

            if (console.IsAsync())
            {
                asyncSinks.emplace_back(std::move(sink));
            }
            else
            {
                syncSinks.emplace_back(std::move(sink));
            }
        }
        {
            constexpr int32_t rotationHour = 0;
            constexpr int32_t rotationMinute = 0;
            constexpr bool truncate = false;

            auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                dailyFile.GetPath().string(), rotationHour, rotationMinute, truncate);
            sink->set_level(ToSpdLog(dailyFile.GetLogLevel()));
            sink->set_pattern(dailyFile.GetPattern());

            if (dailyFile.IsAsync())
            {
                asyncSinks.emplace_back(std::move(sink));
            }
            else
            {
                syncSinks.emplace_back(std::move(sink));
            }
        }

        if (!syncSinks.empty())
        {
            _syncLogger = std::make_shared<spdlog::logger>(CreateUniqueLoggerName(false), syncSinks.begin(), syncSinks.end());
        }

        if (!asyncSinks.empty())
        {
            auto logger = std::make_shared<spdlog::async_logger>(
                CreateUniqueLoggerName(true), asyncSinks.begin(), asyncSinks.end(),
                spdlog::thread_pool(), spdlog::async_overflow_policy::block);

            _asyncLogger = std::move(logger);
            _asyncLogger->flush_on(spdlog::level::info);
        }

        if (_syncLogger)
        {
            spdlog::register_logger(_syncLogger);
        }

        if (_asyncLogger)
        {
            spdlog::register_logger(_asyncLogger);
        }
    }

    SpdLogLogger::~SpdLogLogger()
    {
        if (_syncLogger)
        {
            spdlog::drop(_syncLogger->name());
        }

        if (_asyncLogger)
        {
            spdlog::drop(_asyncLogger->name());
        }
    }

    void SpdLogLogger::Log(LogLevel logLevel, const std::string& message, const std::source_location& location)
    {
        if (_syncLogger)
        {
            _syncLogger->log(ToSpdLog(location), ToSpdLog(logLevel), message);
        }

        if (_asyncLogger)
        {
            _asyncLogger->log(ToSpdLog(location), ToSpdLog(logLevel), message);
        }
    }

    void SpdLogLogger::Flush()
    {
        if (_syncLogger)
        {
            _syncLogger->flush();
        }

        if (_asyncLogger)
        {
            _asyncLogger->flush();
        }
    }

    auto SpdLogLogger::CreateUniqueLoggerName(bool async) -> std::string
    {
        return std::format("{}_{}_logger",
            async ? "async" : "sync", reinterpret_cast<size_t>(this));
    }

    void SpdLogLogger::InitializeSpdLog()
    {
        spdlog::init_thread_pool(8192, 4);
        spdlog::flush_every(std::chrono::seconds(5));
    }
}
