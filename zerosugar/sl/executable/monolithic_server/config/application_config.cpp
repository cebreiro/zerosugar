#include "application_config.h"

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

    void from_json(const nlohmann::json& j, DatabaseConfig& config)
    {
        j.at("address").get_to(config._address);
        j.at("port").get_to(config._port);
        j.at("user").get_to(config._user);
        j.at("password").get_to(config._password);
        j.at("database").get_to(config._database);
        j.at("connection_count").get_to(config._connectionCount);
    }

    void from_json(const nlohmann::json& j, ServerConfig& config)
    {
        config._logConfig = j.at("log").get<LogConfig>();
        config._database = j.at("database").get<DatabaseConfig>();
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

    auto DatabaseConfig::GetAddress() const -> const std::string&
    {
        return _address;
    }

    auto DatabaseConfig::GetPort() const -> uint16_t
    {
        return _port;
    }

    auto DatabaseConfig::GetUser() const -> const std::string&
    {
        return _user;
    }

    auto DatabaseConfig::GetPassword() const -> const std::string&
    {
        return _password;
    }

    auto DatabaseConfig::GetDatabase() const -> const std::string&
    {
        return _database;
    }

    auto DatabaseConfig::GetConnectionCount() const -> int32_t
    {
        return _connectionCount;
    }

    auto ServerConfig::GetLogConfig() const -> const LogConfig&
    {
        return _logConfig;
    }

    auto ServerConfig::GetDatabaseConfig() const -> const DatabaseConfig&
    {
        return _database;
    }
}
