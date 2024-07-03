#include "coordination_command_message_json.h"

namespace zerosugar::xr::coordination::command
{
    void from_json(const nlohmann::json& j, LaunchGameInstance& item)
    {
        j.at("gameInstanceId").get_to(item.gameInstanceId);
        j.at("zoneId").get_to(item.zoneId);
    }

    void to_json(nlohmann::json& j, const LaunchGameInstance& item)
    {
        j = nlohmann::json
            {
                { "gameInstanceId", item.gameInstanceId },
                { "zoneId", item.zoneId },
            };
    }

    void from_json(const nlohmann::json& j, BroadcastChatting& item)
    {
        j.at("message").get_to(item.message);
    }

    void to_json(nlohmann::json& j, const BroadcastChatting& item)
    {
        j = nlohmann::json
            {
                { "message", item.message },
            };
    }

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupCreation& item)
    {
        j.at("userId").get_to(item.userId);
    }

    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupCreation& item)
    {
        j = nlohmann::json
            {
                { "userId", item.userId },
            };
    }

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupApproved& item)
    {
        j.at("userId").get_to(item.userId);
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupApproved& item)
    {
        j = nlohmann::json
            {
                { "userId", item.userId },
                { "ip", item.ip },
                { "port", item.port },
            };
    }

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupRejected& item)
    {
        j.at("userId").get_to(item.userId);
    }

    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupRejected& item)
    {
        j = nlohmann::json
            {
                { "userId", item.userId },
            };
    }

}
