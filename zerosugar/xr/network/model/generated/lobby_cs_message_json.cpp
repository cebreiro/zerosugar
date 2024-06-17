#include "lobby_cs_message_json.h"

namespace zerosugar::xr::network::lobby::cs
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

    void from_json(const nlohmann::json& j, CreateCharacter& item)
    {
        j.at("character").get_to(item.character);
    }

    void to_json(nlohmann::json& j, const CreateCharacter& item)
    {
        j = nlohmann::json
            {
                { "character", item.character },
            };
    }

    void from_json(const nlohmann::json& j, DeleteCharacter& item)
    {
        j.at("slot").get_to(item.slot);
    }

    void to_json(nlohmann::json& j, const DeleteCharacter& item)
    {
        j = nlohmann::json
            {
                { "slot", item.slot },
            };
    }

    void from_json(const nlohmann::json& j, SelectCharacter& item)
    {
        j.at("slot").get_to(item.slot);
    }

    void to_json(nlohmann::json& j, const SelectCharacter& item)
    {
        j = nlohmann::json
            {
                { "slot", item.slot },
            };
    }

}
