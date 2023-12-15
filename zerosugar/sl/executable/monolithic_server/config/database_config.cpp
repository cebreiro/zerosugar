#include "database_config.h"

namespace zerosugar::sl
{
    void from_json(const nlohmann::json& j, DatabaseConfig& config)
    {
        j.at("address").get_to(config._address);
        j.at("port").get_to(config._port);
        j.at("user").get_to(config._user);
        j.at("password").get_to(config._password);
        j.at("database").get_to(config._database);
        j.at("connection_count").get_to(config._connectionCount);
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
}
