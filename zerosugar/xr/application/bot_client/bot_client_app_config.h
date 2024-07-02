#pragma once

namespace zerosugar::xr
{
    struct BotClientAppConfig
    {
        int64_t workerCount = std::thread::hardware_concurrency();
        std::string logFilePath;

        std::string loginIP;
        uint16_t loginPort = 0;

        std::string behaviorTreeDataDirectoryPath;
        std::string navigationDataDirectoryPath;

        friend void from_json(const nlohmann::json& j, BotClientAppConfig& config)
        {
            config.workerCount = j.at("executor_thread_count").get<int64_t>();

            config.logFilePath = j.at("log_file_path").get<std::string>();

            config.loginIP = j.at("login_ip").get<std::string>();
            config.loginPort = j.at("login_port").get<uint16_t>();

            config.behaviorTreeDataDirectoryPath = j.at("behavior_tree_data_directory_path").get<std::string>();
            config.navigationDataDirectoryPath = j.at("navigation_data_directory_path").get<std::string>();
        }
    };
}
