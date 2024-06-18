#include "coordination_service_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, RegisterServerParam& item)
    {
        j.at("name").get_to(item.name);
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const RegisterServerParam& item)
    {
        j = nlohmann::json
            {
                { "name", item.name },
                { "ip", item.ip },
                { "port", item.port },
            };
    }

    void from_json(const nlohmann::json& j, RegisterServerResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("serverId").get_to(item.serverId);
    }

    void to_json(nlohmann::json& j, const RegisterServerResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "serverId", item.serverId },
            };
    }

    void from_json(const nlohmann::json& j, CoordinationChannelInput& item)
    {
        j.at("opcode").get_to(item.opcode);
        j.at("bytes").get_to(item.bytes);
    }

    void to_json(nlohmann::json& j, const CoordinationChannelInput& item)
    {
        j = nlohmann::json
            {
                { "opcode", item.opcode },
                { "bytes", item.bytes },
            };
    }

    void from_json(const nlohmann::json& j, CoordinationChannelOutput& item)
    {
        j.at("opcode").get_to(item.opcode);
        j.at("bytes").get_to(item.bytes);
    }

    void to_json(nlohmann::json& j, const CoordinationChannelOutput& item)
    {
        j = nlohmann::json
            {
                { "opcode", item.opcode },
                { "bytes", item.bytes },
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

    void from_json(const nlohmann::json& j, AddPlayerParam& item)
    {
        j.at("authenticationToken").get_to(item.authenticationToken);
        j.at("accountId").get_to(item.accountId);
        j.at("characterId").get_to(item.characterId);
        j.at("zoneId").get_to(item.zoneId);
    }

    void to_json(nlohmann::json& j, const AddPlayerParam& item)
    {
        j = nlohmann::json
            {
                { "authenticationToken", item.authenticationToken },
                { "accountId", item.accountId },
                { "characterId", item.characterId },
                { "zoneId", item.zoneId },
            };
    }

    void from_json(const nlohmann::json& j, AddPlayerResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const AddPlayerResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "ip", item.ip },
                { "port", item.port },
            };
    }

}
