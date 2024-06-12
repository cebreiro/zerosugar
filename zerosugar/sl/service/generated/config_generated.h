#pragma once
#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace zerosugar::sl::service
{
    struct DatabaseConfig
    {
        std::string address = {};
        int32_t port = {};
        std::string user = {};
        std::string password = {};
        std::string database = {};
    };

    struct RepositoryServiceConfig
    {
        DatabaseConfig databaseConfig = {};
    };

}
