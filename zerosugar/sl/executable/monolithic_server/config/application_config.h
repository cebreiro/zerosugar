#pragma once
#include "zerosugar/sl/executable/monolithic_server/config/log_config.h"
#include "zerosugar/sl/executable/monolithic_server/config/database_config.h"
#include "zerosugar/sl/executable/monolithic_server/config/server_config.h"

namespace zerosugar::sl
{
    class AppConfig
    {
        friend void from_json(const nlohmann::json& j, AppConfig& config);

    public:
        AppConfig() = default;

        auto GetLogConfig() const -> const LogConfig&;
        auto GetDatabaseConfig() const -> const DatabaseConfig&;
        auto GetServerConfig() const -> const ServerConfig&;

    private:
        LogConfig _logConfig;
        DatabaseConfig _database;
        ServerConfig _serverConfig;
    };
}
