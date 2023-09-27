#include "spdlog_serivce_build_config.h"

namespace zerosugar
{
    SpdLogConsoleLogConfig& SpdLogConsoleLogConfig::SetColor(LogLevel logLevel, uint16_t color)
    {
        _colors[logLevel] = color;

        return *this;
    }

    auto SpdLogConsoleLogConfig::GetColors() const noexcept -> const std::map<LogLevel, uint16_t>&
    {
        return _colors;
    }

    SpdLogDailyFileLogConfig& SpdLogDailyFileLogConfig::SetPath(std::filesystem::path path)
    {
        _path = std::move(path);

        return *this;
    }
}
