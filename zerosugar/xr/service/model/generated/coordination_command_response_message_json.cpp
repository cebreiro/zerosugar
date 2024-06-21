#include "coordination_command_response_message_json.h"

namespace zerosugar::xr::coordination::command::response
{
    void from_json(const nlohmann::json& j, Authenticate& item)
    {
        j.at("serverId").get_to(item.serverId);
    }

    void to_json(nlohmann::json& j, const Authenticate& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
            };
    }

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

}
