#include "game_sc_message_json.h"

namespace zerosugar::xr::network::game::sc
{
    void from_json(const nlohmann::json& j, EnterGame& item)
    {
        j.at("zoneId").get_to(item.zoneId);
        j.at("remotePlayersCount").get_to(item.remotePlayersCount);
        j.at("remotePlayers").get_to(item.remotePlayers);
        j.at("monstersCount").get_to(item.monstersCount);
        j.at("monsters").get_to(item.monsters);
        j.at("localPlayer").get_to(item.localPlayer);
    }

    void to_json(nlohmann::json& j, const EnterGame& item)
    {
        j = nlohmann::json
            {
                { "zoneId", item.zoneId },
                { "remotePlayersCount", item.remotePlayersCount },
                { "remotePlayers", item.remotePlayers },
                { "monstersCount", item.monstersCount },
                { "monsters", item.monsters },
                { "localPlayer", item.localPlayer },
            };
    }

    void from_json(const nlohmann::json& j, AddRemotePlayer& item)
    {
        j.at("player").get_to(item.player);
    }

    void to_json(nlohmann::json& j, const AddRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "player", item.player },
            };
    }

    void from_json(const nlohmann::json& j, RemoveRemotePlayer& item)
    {
        j.at("id").get_to(item.id);
    }

    void to_json(nlohmann::json& j, const RemoveRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
            };
    }

    void from_json(const nlohmann::json& j, MoveRemotePlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const MoveRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "position", item.position },
            };
    }

}
