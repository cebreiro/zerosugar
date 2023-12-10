#pragma once
#include <nlohmann/json.hpp>

namespace zerosugar::sl
{
    class LogConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, LogConfig& config);

    public:
        class Console
        {
        private:
            friend void from_json(const nlohmann::json& j, Console& config);

        public:
            bool ShouldAsync() const;
            auto GetLevel() const -> int32_t;

        private:
            bool _async = false;
            int32_t _level = 1;
        };

        class DailyFile
        {
        private:
            friend void from_json(const nlohmann::json& j, DailyFile& config);

        public:
            auto GetFilePath() const -> const std::filesystem::path&;

        private:
            std::filesystem::path _filePath;
        };

    public:
        LogConfig() = default;

        auto GetConsole() const -> const Console&;
        auto GetDailyFile() const -> const DailyFile&;

    private:
        Console _console;
        DailyFile _dailyFile;
    };

    class DatabaseConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, DatabaseConfig& config);

    public:
        DatabaseConfig() = default;

        auto GetAddress() const -> const std::string&;
        auto GetPort() const -> uint16_t;
        auto GetUser() const -> const std::string&;
        auto GetPassword() const -> const std::string&;
        auto GetDatabase() const -> const std::string&;
        auto GetConnectionCount() const -> int32_t;

    private:
        std::string _address;
        uint16_t _port = 0;
        std::string _user;
        std::string _password;
        std::string _database;
        int32_t _connectionCount;
    };

    class ServerConfig
    {
        friend void from_json(const nlohmann::json& j, ServerConfig& config);

    public:
        ServerConfig() = default;

        auto GetLogConfig() const -> const LogConfig&;
        auto GetDatabaseConfig() const -> const DatabaseConfig&;

    private:
        LogConfig _logConfig;
        DatabaseConfig _database;
    };
}
