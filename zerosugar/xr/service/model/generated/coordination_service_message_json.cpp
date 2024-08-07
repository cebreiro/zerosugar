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

    void from_json(const nlohmann::json& j, UpdateServerStatusParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("loadCPUPercentage").get_to(item.loadCPUPercentage);
        j.at("freePhysicalMemoryGB").get_to(item.freePhysicalMemoryGB);
    }

    void to_json(nlohmann::json& j, const UpdateServerStatusParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "loadCPUPercentage", item.loadCPUPercentage },
                { "freePhysicalMemoryGB", item.freePhysicalMemoryGB },
            };
    }

    void from_json(const nlohmann::json& j, UpdateServerStatusResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const UpdateServerStatusResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, CoordinationCommandResponse& item)
    {
        j.at("responseId").get_to(item.responseId);
        j.at("opcode").get_to(item.opcode);
        j.at("contents").get_to(item.contents);
    }

    void to_json(nlohmann::json& j, const CoordinationCommandResponse& item)
    {
        j = nlohmann::json
            {
                { "responseId", item.responseId },
                { "opcode", item.opcode },
                { "contents", item.contents },
            };
    }

    void from_json(const nlohmann::json& j, CoordinationCommand& item)
    {
        if (const auto iter = j.find("responseId"); iter != j.end())
        {
            item.responseId.emplace(*iter);
        }
        j.at("opcode").get_to(item.opcode);
        j.at("contents").get_to(item.contents);
    }

    void to_json(nlohmann::json& j, const CoordinationCommand& item)
    {
        j = nlohmann::json
            {
                { "opcode", item.opcode },
                { "contents", item.contents },
            };

        if (item.responseId.has_value())
        {
            j["responseId"] = *item.responseId;
        }
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

    void from_json(const nlohmann::json& j, RemovePlayerParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const RemovePlayerParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, RemovePlayerResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const RemovePlayerResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, AuthenticatePlayerParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const AuthenticatePlayerParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, AuthenticatePlayerResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("accountId").get_to(item.accountId);
        j.at("characterId").get_to(item.characterId);
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("userUniqueId").get_to(item.userUniqueId);
    }

    void to_json(nlohmann::json& j, const AuthenticatePlayerResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "accountId", item.accountId },
                { "characterId", item.characterId },
                { "gameInstanceId", item.gameInstanceId },
                { "userUniqueId", item.userUniqueId },
            };
    }

    void from_json(const nlohmann::json& j, RemoveGameInstanceParam& item)
    {
        j.at("gameInstanceId").get_to(item.gameInstanceId);
    }

    void to_json(nlohmann::json& j, const RemoveGameInstanceParam& item)
    {
        j = nlohmann::json
            {
                { "gameInstanceId", item.gameInstanceId },
            };
    }

    void from_json(const nlohmann::json& j, RemoveGameInstanceResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const RemoveGameInstanceResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, BroadcastChattingParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("authenticationToken").get_to(item.authenticationToken);
        j.at("message").get_to(item.message);
    }

    void to_json(nlohmann::json& j, const BroadcastChattingParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "gameInstanceId", item.gameInstanceId },
                { "authenticationToken", item.authenticationToken },
                { "message", item.message },
            };
    }

    void from_json(const nlohmann::json& j, BroadcastChattingResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const BroadcastChattingResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, RequestDungeonMatchParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("authenticationToken").get_to(item.authenticationToken);
        j.at("dungeonId").get_to(item.dungeonId);
    }

    void to_json(nlohmann::json& j, const RequestDungeonMatchParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "gameInstanceId", item.gameInstanceId },
                { "authenticationToken", item.authenticationToken },
                { "dungeonId", item.dungeonId },
            };
    }

    void from_json(const nlohmann::json& j, RequestDungeonMatchResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const RequestDungeonMatchResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, CancelDungeonMatchParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const CancelDungeonMatchParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "gameInstanceId", item.gameInstanceId },
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, CancelDungeonMatchResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const CancelDungeonMatchResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, ApproveDungeonMatchParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const ApproveDungeonMatchParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "gameInstanceId", item.gameInstanceId },
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, ApproveDungeonMatchResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const ApproveDungeonMatchResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, RejectDungeonMatchParam& item)
    {
        j.at("serverId").get_to(item.serverId);
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const RejectDungeonMatchParam& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
                { "gameInstanceId", item.gameInstanceId },
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, RejectDungeonMatchResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const RejectDungeonMatchResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

}
