#include "game_cs_message_json.h"

namespace zerosugar::xr::network::game::cs
{
    void from_json(const nlohmann::json& j, Authenticate& item)
    {
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const Authenticate& item)
    {
        j = nlohmann::json
            {
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, MovePlayer& item)
    {
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const MovePlayer& item)
    {
        j = nlohmann::json
            {
                { "position", item.position },
            };
    }

    void from_json(const nlohmann::json& j, StopPlayerMovement& item)
    {
        j.at("id").get_to(item.id);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const StopPlayerMovement& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "position", item.position },
            };
    }

}
