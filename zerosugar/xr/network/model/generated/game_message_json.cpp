#include "game_message_json.h"

namespace zerosugar::xr::network::game
{
    void from_json(const nlohmann::json& j, Position& item)
    {
        j.at("x").get_to(item.x);
        j.at("y").get_to(item.y);
        j.at("z").get_to(item.z);
    }

    void to_json(nlohmann::json& j, const Position& item)
    {
        j = nlohmann::json
            {
                { "x", item.x },
                { "y", item.y },
                { "z", item.z },
            };
    }

    void from_json(const nlohmann::json& j, Rotation& item)
    {
        j.at("yaw").get_to(item.yaw);
        j.at("pitch").get_to(item.pitch);
        j.at("roll").get_to(item.roll);
    }

    void to_json(nlohmann::json& j, const Rotation& item)
    {
        j = nlohmann::json
            {
                { "yaw", item.yaw },
                { "pitch", item.pitch },
                { "roll", item.roll },
            };
    }

    void from_json(const nlohmann::json& j, Transform& item)
    {
        j.at("position").get_to(item.position);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const Transform& item)
    {
        j = nlohmann::json
            {
                { "position", item.position },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, CharacterStat& item)
    {
        j.at("hp").get_to(item.hp);
        j.at("maxHP").get_to(item.maxHP);
        j.at("attackMin").get_to(item.attackMin);
        j.at("attackMax").get_to(item.attackMax);
        j.at("attackRange").get_to(item.attackRange);
        j.at("attackSpeed").get_to(item.attackSpeed);
        j.at("defence").get_to(item.defence);
        j.at("name").get_to(item.name);
        j.at("level").get_to(item.level);
        j.at("gender").get_to(item.gender);
        j.at("face").get_to(item.face);
        j.at("hair").get_to(item.hair);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
        j.at("stamina").get_to(item.stamina);
        j.at("staminaMax").get_to(item.staminaMax);
    }

    void to_json(nlohmann::json& j, const CharacterStat& item)
    {
        j = nlohmann::json
            {
                { "hp", item.hp },
                { "maxHP", item.maxHP },
                { "attackMin", item.attackMin },
                { "attackMax", item.attackMax },
                { "attackRange", item.attackRange },
                { "attackSpeed", item.attackSpeed },
                { "defence", item.defence },
                { "name", item.name },
                { "level", item.level },
                { "gender", item.gender },
                { "face", item.face },
                { "hair", item.hair },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
                { "stamina", item.stamina },
                { "staminaMax", item.staminaMax },
            };
    }

    void from_json(const nlohmann::json& j, Equipment& item)
    {
        j.at("type").get_to(item.type);
        j.at("id").get_to(item.id);
        j.at("attack").get_to(item.attack);
        j.at("defence").get_to(item.defence);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
    }

    void to_json(nlohmann::json& j, const Equipment& item)
    {
        j = nlohmann::json
            {
                { "type", item.type },
                { "id", item.id },
                { "attack", item.attack },
                { "defence", item.defence },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
            };
    }

    void from_json(const nlohmann::json& j, CharacterEquipment& item)
    {
        j.at("armor").get_to(item.armor);
        j.at("gloves").get_to(item.gloves);
        j.at("shoes").get_to(item.shoes);
        j.at("weapon").get_to(item.weapon);
    }

    void to_json(nlohmann::json& j, const CharacterEquipment& item)
    {
        j = nlohmann::json
            {
                { "armor", item.armor },
                { "gloves", item.gloves },
                { "shoes", item.shoes },
                { "weapon", item.weapon },
            };
    }

    void from_json(const nlohmann::json& j, CharacterInventoryItem& item)
    {
        j.at("id").get_to(item.id);
        j.at("count").get_to(item.count);
        j.at("attack").get_to(item.attack);
        j.at("defence").get_to(item.defence);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
    }

    void to_json(nlohmann::json& j, const CharacterInventoryItem& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "count", item.count },
                { "attack", item.attack },
                { "defence", item.defence },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
            };
    }

    void from_json(const nlohmann::json& j, Character& item)
    {
        j.at("instanceId").get_to(item.instanceId);
        j.at("transform").get_to(item.transform);
        j.at("stat").get_to(item.stat);
        j.at("equipment").get_to(item.equipment);
        j.at("gold").get_to(item.gold);
        j.at("items").get_to(item.items);
    }

    void to_json(nlohmann::json& j, const Character& item)
    {
        j = nlohmann::json
            {
                { "instanceId", item.instanceId },
                { "transform", item.transform },
                { "stat", item.stat },
                { "equipment", item.equipment },
                { "gold", item.gold },
                { "items", item.items },
            };
    }

    void from_json(const nlohmann::json& j, Monster& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const Monster& item)
    {
        (void)j;
        (void)item;
    }

}
