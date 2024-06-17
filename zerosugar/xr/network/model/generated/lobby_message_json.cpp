#include "lobby_message_json.h"

namespace zerosugar::xr::network::lobby
{
    void from_json(const nlohmann::json& j, LobbyCharacter& item)
    {
        j.at("slot").get_to(item.slot);
        j.at("name").get_to(item.name);
        j.at("level").get_to(item.level);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
        j.at("job").get_to(item.job);
        j.at("faceId").get_to(item.faceId);
        j.at("hairId").get_to(item.hairId);
        j.at("gold").get_to(item.gold);
        j.at("zoneId").get_to(item.zoneId);
        j.at("x").get_to(item.x);
        j.at("y").get_to(item.y);
        j.at("z").get_to(item.z);
        j.at("armorId").get_to(item.armorId);
        j.at("glovesId").get_to(item.glovesId);
        j.at("shoesId").get_to(item.shoesId);
        j.at("weaponId").get_to(item.weaponId);
        j.at("gender").get_to(item.gender);
    }

    void to_json(nlohmann::json& j, const LobbyCharacter& item)
    {
        j = nlohmann::json
            {
                { "slot", item.slot },
                { "name", item.name },
                { "level", item.level },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
                { "job", item.job },
                { "faceId", item.faceId },
                { "hairId", item.hairId },
                { "gold", item.gold },
                { "zoneId", item.zoneId },
                { "x", item.x },
                { "y", item.y },
                { "z", item.z },
                { "armorId", item.armorId },
                { "glovesId", item.glovesId },
                { "shoesId", item.shoesId },
                { "weaponId", item.weaponId },
                { "gender", item.gender },
            };
    }

}
