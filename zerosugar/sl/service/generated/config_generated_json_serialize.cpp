#include "config_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, DatabaseConfig& item)
    {
        j.at("address").get_to(item.address);
        j.at("port").get_to(item.port);
        j.at("user").get_to(item.user);
        j.at("password").get_to(item.password);
        j.at("database").get_to(item.database);
    }

    void to_json(nlohmann::json& j, const DatabaseConfig& item)
    {
        j = nlohmann::json
            {
                { "address", item.address },
                { "port", item.port },
                { "user", item.user },
                { "password", item.password },
                { "database", item.database },
            };
    }

    void from_json(const nlohmann::json& j, RepositoryServiceConfig& item)
    {
        j.at("databaseConfig").get_to(item.databaseConfig);
    }

    void to_json(nlohmann::json& j, const RepositoryServiceConfig& item)
    {
        j = nlohmann::json
            {
                { "databaseConfig", item.databaseConfig },
            };
    }

}
