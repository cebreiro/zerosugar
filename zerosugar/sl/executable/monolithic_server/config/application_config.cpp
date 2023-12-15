#include "application_config.h"

namespace zerosugar::sl
{
    void from_json(const nlohmann::json& j, AppConfig& config)
    {
        config._logConfig = j.at("log").get<LogConfig>();
        config._database = j.at("database").get<DatabaseConfig>();
        config._serverConfig = j.at("server").get<ServerConfig>();
    }

    auto AppConfig::GetLogConfig() const -> const LogConfig&
    {
        return _logConfig;
    }

    auto AppConfig::GetDatabaseConfig() const -> const DatabaseConfig&
    {
        return _database;
    }

    auto AppConfig::GetServerConfig() const -> const ServerConfig&
    {
        return _serverConfig;
    }
}
