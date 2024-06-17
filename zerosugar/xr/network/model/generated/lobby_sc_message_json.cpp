#include "lobby_sc_message_json.h"

namespace zerosugar::xr::network::lobby::sc
{
    void from_json(const nlohmann::json& j, FailAuthenticate& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const FailAuthenticate& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, ResultCreateCharacter& item)
    {
        j.at("success").get_to(item.success);
        j.at("character").get_to(item.character);
    }

    void to_json(nlohmann::json& j, const ResultCreateCharacter& item)
    {
        j = nlohmann::json
            {
                { "success", item.success },
                { "character", item.character },
            };
    }

    void from_json(const nlohmann::json& j, SuccessDeleteCharacter& item)
    {
        j.at("slot").get_to(item.slot);
    }

    void to_json(nlohmann::json& j, const SuccessDeleteCharacter& item)
    {
        j = nlohmann::json
            {
                { "slot", item.slot },
            };
    }

    void from_json(const nlohmann::json& j, NotifyCharacterList& item)
    {
        j.at("count").get_to(item.count);
        j.at("character").get_to(item.character);
    }

    void to_json(nlohmann::json& j, const NotifyCharacterList& item)
    {
        j = nlohmann::json
            {
                { "count", item.count },
                { "character", item.character },
            };
    }

    void from_json(const nlohmann::json& j, SuccessSelectCharacter& item)
    {
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const SuccessSelectCharacter& item)
    {
        j = nlohmann::json
            {
                { "ip", item.ip },
                { "port", item.port },
            };
    }

}
