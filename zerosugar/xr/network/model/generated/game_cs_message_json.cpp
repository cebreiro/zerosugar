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
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const MovePlayer& item)
    {
        j = nlohmann::json
            {
                { "position", item.position },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, StopPlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const StopPlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "position", item.position },
            };
    }

    void from_json(const nlohmann::json& j, SprintPlayer& item)
    {
        j.at("id").get_to(item.id);
    }

    void to_json(nlohmann::json& j, const SprintPlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
            };
    }

    void from_json(const nlohmann::json& j, RollDodgePlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const RollDodgePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, Chat& item)
    {
        j.at("message").get_to(item.message);
    }

    void to_json(nlohmann::json& j, const Chat& item)
    {
        j = nlohmann::json
            {
                { "message", item.message },
            };
    }

    void from_json(const nlohmann::json& j, SwapItem& item)
    {
        j.at("destEquipped").get_to(item.destEquipped);
        j.at("destPosition").get_to(item.destPosition);
        j.at("srcEquipped").get_to(item.srcEquipped);
        j.at("srcPosition").get_to(item.srcPosition);
    }

    void to_json(nlohmann::json& j, const SwapItem& item)
    {
        j = nlohmann::json
            {
                { "destEquipped", item.destEquipped },
                { "destPosition", item.destPosition },
                { "srcEquipped", item.srcEquipped },
                { "srcPosition", item.srcPosition },
            };
    }

}
