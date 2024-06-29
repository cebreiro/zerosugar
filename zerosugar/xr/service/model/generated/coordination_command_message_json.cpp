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

}
