#include "game_service_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, GetNameParam& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const GetNameParam& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, GetNameResult& item)
    {
        j.at("name").get_to(item.name);
    }

    void to_json(nlohmann::json& j, const GetNameResult& item)
    {
        j = nlohmann::json
            {
                { "name", item.name },
            };
    }

    void from_json(const nlohmann::json& j, RequestSnowflakeKeyParam& item)
    {
        j.at("requester").get_to(item.requester);
    }

    void to_json(nlohmann::json& j, const RequestSnowflakeKeyParam& item)
    {
        j = nlohmann::json
            {
                { "requester", item.requester },
            };
    }

    void from_json(const nlohmann::json& j, RequestSnowflakeKeyResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("snowflakeKey").get_to(item.snowflakeKey);
    }

    void to_json(nlohmann::json& j, const RequestSnowflakeKeyResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "snowflakeKey", item.snowflakeKey },
            };
    }

    void from_json(const nlohmann::json& j, ReturnSnowflakeKeyParam& item)
    {
        j.at("requester").get_to(item.requester);
        j.at("snowflakeKey").get_to(item.snowflakeKey);
    }

    void to_json(nlohmann::json& j, const ReturnSnowflakeKeyParam& item)
    {
        j = nlohmann::json
            {
                { "requester", item.requester },
                { "snowflakeKey", item.snowflakeKey },
            };
    }

    void from_json(const nlohmann::json& j, ReturnSnowflakeKeyResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const ReturnSnowflakeKeyResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

}
