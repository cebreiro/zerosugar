#pragma once

namespace zerosugar::sl
{
    class DatabaseConfig
    {
    private:
        friend void from_json(const nlohmann::json& j, DatabaseConfig& config);

    public:
        DatabaseConfig() = default;

        auto GetAddress() const -> const std::string&;
        auto GetPort() const->uint16_t;
        auto GetUser() const -> const std::string&;
        auto GetPassword() const -> const std::string&;
        auto GetDatabase() const -> const std::string&;
        auto GetConnectionCount() const->int32_t;

    private:
        std::string _address;
        uint16_t _port = 0;
        std::string _user;
        std::string _password;
        std::string _database;
        int32_t _connectionCount;
    };
}
