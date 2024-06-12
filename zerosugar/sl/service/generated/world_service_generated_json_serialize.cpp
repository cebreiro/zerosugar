#include "world_service_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, World& item)
    {
        j.at("worldId").get_to(item.worldId);
        j.at("address").get_to(item.address);
    }

    void to_json(nlohmann::json& j, const World& item)
    {
        j = nlohmann::json
            {
                { "worldId", item.worldId },
                { "address", item.address },
            };
    }

    void from_json(const nlohmann::json& j, CreateWorldParam& item)
    {
        j.at("worldId").get_to(item.worldId);
        j.at("address").get_to(item.address);
    }

    void to_json(nlohmann::json& j, const CreateWorldParam& item)
    {
        j = nlohmann::json
            {
                { "worldId", item.worldId },
                { "address", item.address },
            };
    }

    void from_json(const nlohmann::json& j, CreateWorldResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const CreateWorldResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, CreateZoneParam& item)
    {
        j.at("worldId").get_to(item.worldId);
        j.at("zoneId").get_to(item.zoneId);
        j.at("address").get_to(item.address);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const CreateZoneParam& item)
    {
        j = nlohmann::json
            {
                { "worldId", item.worldId },
                { "zoneId", item.zoneId },
                { "address", item.address },
                { "port", item.port },
            };
    }

    void from_json(const nlohmann::json& j, CreateZoneResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const CreateZoneResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, GetWorldListParam& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const GetWorldListParam& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, GetWorldListResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("worlds").get_to(item.worlds);
    }

    void to_json(nlohmann::json& j, const GetWorldListResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "worlds", item.worlds },
            };
    }

    void from_json(const nlohmann::json& j, EnterWorldParam& item)
    {
        j.at("worldId").get_to(item.worldId);
        j.at("token").get_to(item.token);
        j.at("accountId").get_to(item.accountId);
        j.at("account").get_to(item.account);
    }

    void to_json(nlohmann::json& j, const EnterWorldParam& item)
    {
        j = nlohmann::json
            {
                { "worldId", item.worldId },
                { "token", item.token },
                { "accountId", item.accountId },
                { "account", item.account },
            };
    }

    void from_json(const nlohmann::json& j, EnterWorldResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const EnterWorldResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, KickPlayerParam& item)
    {
        j.at("token").get_to(item.token);
        if (const auto iter = j.find("displayReason"); iter != j.end())
        {
            item.displayReason.emplace(*iter);
        }
    }

    void to_json(nlohmann::json& j, const KickPlayerParam& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
            };

        if (item.displayReason.has_value())
        {
            j.push_back(nlohmann::json{ "displayReason", *item.displayReason });
        }
    }

    void from_json(const nlohmann::json& j, KickPlayerResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const KickPlayerResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

}
