#include "log_config.h"

namespace zerosugar::sl
{
    void from_json(const nlohmann::json& j, LogConfig::Console& config)
    {
        j.at("async").get_to(config._async);
        j.at("level").get_to(config._level);
    }

    void from_json(const nlohmann::json& j, LogConfig::DailyFile& config)
    {
        j.at("path").get_to(config._filePath);
    }

    void from_json(const nlohmann::json& j, LogConfig& config)
    {
        j.at("console").get_to(config._console);
        j.at("file").get_to(config._dailyFile);
    }

    bool LogConfig::Console::ShouldAsync() const
    {
        return _async;
    }

    auto LogConfig::Console::GetLevel() const -> int32_t
    {
        return _level;
    }

    auto LogConfig::DailyFile::GetFilePath() const -> const std::filesystem::path&
    {
        return _filePath;
    }

    auto LogConfig::GetConsole() const -> const Console&
    {
        return _console;
    }

    auto LogConfig::GetDailyFile() const -> const DailyFile&
    {
        return _dailyFile;
    }
}
