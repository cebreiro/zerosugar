#include "spdlog_service_builder.h"

namespace zerosugar
{
    void SpdLogServiceBuilder::SetConsoleConfig(SpdLogConsoleLogConfig config)
    {
        _consoleLogConfig = std::move(config);
    }

    void SpdLogServiceBuilder::SetDailyFileLogConfig(SpdLogDailyFileLogConfig config)
    {
        _dailyFileLogConfig = std::move(config);
    }

    auto SpdLogServiceBuilder::ConfigureConsoleLog() -> SpdLogConsoleLogConfig&
    {
        if (_consoleLogConfig.has_value())
        {
            _consoleLogConfig.reset();
        }

        return _consoleLogConfig.emplace();
    }

    auto SpdLogServiceBuilder::ConfigureDailyFileLog() -> SpdLogDailyFileLogConfig&
    {
        if (_dailyFileLogConfig.has_value())
        {
            _dailyFileLogConfig.reset();
        }

        return _dailyFileLogConfig.emplace();
    }
}
