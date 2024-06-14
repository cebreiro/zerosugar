#pragma once

namespace zerosugar::xr
{
    struct AllInOneAppConfig
    {
        int64_t workerCount = std::thread::hardware_concurrency();
        std::string logFilePath;
        std::string rpcServerIP;
        uint16_t rpcServerPort;
        std::string databaseIP;
        uint16_t databasePort = 0;
        std::string databaseUser;
        std::string databasePassword;
        std::string databaseSchema;
        int32_t databaseConnectionCount;

        friend void from_json(const nlohmann::json& j, AllInOneAppConfig& config)
        {
            config.workerCount = j.at("executor_thread_count").get<int64_t>();
            config.logFilePath = j.at("log_file_path").get<std::string>();
            config.rpcServerIP = j.at("rpc_server_ip").get<std::string>();
            config.rpcServerPort = j.at("rpc_server_port").get<uint16_t>();
            config.databaseIP = j.at("database_ip").get<std::string>();
            config.databasePort = j.at("database_port").get<uint16_t>();
            config.databaseUser = j.at("database_user").get<std::string>();
            config.databasePassword = j.at("database_password").get<std::string>();
            config.databaseSchema = j.at("database_schema").get<std::string>();
            config.databaseConnectionCount = j.at("database_connection_count").get<int32_t>();
        }
    };
}
