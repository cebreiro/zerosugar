#include "gateway_service_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, GameServiceEndpoint& item)
    {
        j.at("name").get_to(item.name);
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const GameServiceEndpoint& item)
    {
        j = nlohmann::json
            {
                { "name", item.name },
                { "ip", item.ip },
                { "port", item.port },
            };
    }

    void from_json(const nlohmann::json& j, AddGameServiceParam& item)
    {
        j.at("address").get_to(item.address);
    }

    void to_json(nlohmann::json& j, const AddGameServiceParam& item)
    {
        j = nlohmann::json
            {
                { "address", item.address },
            };
    }

    void from_json(const nlohmann::json& j, AddGameServiceResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const AddGameServiceResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, GetGameServiceListParam& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const GetGameServiceListParam& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, GetGameServiceListResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("addresses").get_to(item.addresses);
    }

    void to_json(nlohmann::json& j, const GetGameServiceListResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "addresses", item.addresses },
            };
    }

}
