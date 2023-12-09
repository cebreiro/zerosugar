#include "spdlog_logger_builder.h"

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "zerosugar/shared/log/spdlog/spdlog_convert.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_build_config.h"

namespace zerosugar
{
    std::once_flag SpdLogLoggerBuilder::_initFlag;

    SpdLogLoggerBuilder::SpdLogLoggerBuilder()
    {
        std::call_once(_initFlag, InitializeSpdLog);
    }

    void SpdLogLoggerBuilder::SetConfig(const SpdLogConsoleLoggerConfig& config)
    {
        _consoleConfig = config;
    }

    void SpdLogLoggerBuilder::SetConfig(const SpdLogDailyFileLoggerConfig& config)
    {
        _dailyFileConfig = config;
    }

    auto SpdLogLoggerBuilder::ConfigureConsole() -> SpdLogConsoleLoggerConfig&
    {
        return _consoleConfig.emplace();
    }

    auto SpdLogLoggerBuilder::ConfigureDailyFile() -> SpdLogDailyFileLoggerConfig&
    {
        return _dailyFileConfig.emplace();
    }

    auto SpdLogLoggerBuilder::CreateLogger() const -> SharedPtrNotNull<SpdLogLogger>
    {
        static std::atomic<int64_t> counter = 0;
        auto makeLoggerName = [&]() -> std::string
            {
                return std::format("logger[{}]", ++counter);
            };

        std::shared_ptr<spdlog::logger> syncLogger;
        std::shared_ptr<spdlog::async_logger> asyncLogger;
        std::vector<std::shared_ptr<spdlog::sinks::sink>> syncSinks;
        std::vector<std::shared_ptr<spdlog::sinks::sink>> asyncSinks;

        if (_consoleConfig.has_value())
        {
            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sink->set_level(detail::ToSpdLog(_consoleConfig->GetLogLevel()));
            sink->set_pattern(_consoleConfig->GetPattern());

            for (const auto& [level, color] : _consoleConfig->GetColors())
            {
                sink->set_color(detail::ToSpdLog(level), color);
            }

            if (_consoleConfig->IsAsync())
            {
                asyncSinks.emplace_back(std::move(sink));
            }
            else
            {
                syncSinks.emplace_back(std::move(sink));
            }
        }

        if (_dailyFileConfig.has_value())
        {
            constexpr int32_t rotationHour = 0;
            constexpr int32_t rotationMinute = 0;
            constexpr bool truncate = false;

            auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                _dailyFileConfig->GetPath().string(), rotationHour, rotationMinute, truncate);
            sink->set_level(detail::ToSpdLog(_dailyFileConfig->GetLogLevel()));
            sink->set_pattern(_dailyFileConfig->GetPattern());

            if (_dailyFileConfig->IsAsync())
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
            syncLogger = std::make_shared<spdlog::logger>(makeLoggerName(), syncSinks.begin(), syncSinks.end());
        }

        if (!asyncSinks.empty())
        {
            auto logger = std::make_shared<spdlog::async_logger>(
                makeLoggerName(), asyncSinks.begin(), asyncSinks.end(),
                spdlog::thread_pool(), spdlog::async_overflow_policy::block);

            asyncLogger = std::move(logger);
            asyncLogger->flush_on(spdlog::level::info);
        }

        return std::make_shared<SpdLogLogger>(std::move(syncLogger), std::move(asyncLogger));
    }

    void SpdLogLoggerBuilder::InitializeSpdLog()
    {
        spdlog::init_thread_pool(8192, 4);
        spdlog::flush_every(std::chrono::seconds(5));
    }
}
