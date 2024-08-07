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
        j.at("pitch").get_to(item.pitch);
        j.at("yaw").get_to(item.yaw);
        j.at("roll").get_to(item.roll);
    }

    void to_json(nlohmann::json& j, const Rotation& item)
    {
        j = nlohmann::json
            {
                { "pitch", item.pitch },
                { "yaw", item.yaw },
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

    void from_json(const nlohmann::json& j, PlayerBase& item)
    {
        j.at("hp").get_to(item.hp);
        j.at("maxHp").get_to(item.maxHp);
        j.at("attackMin").get_to(item.attackMin);
        j.at("attackMax").get_to(item.attackMax);
        j.at("attackRange").get_to(item.attackRange);
        j.at("attackSpeed").get_to(item.attackSpeed);
        j.at("speed").get_to(item.speed);
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

    void to_json(nlohmann::json& j, const PlayerBase& item)
    {
        j = nlohmann::json
            {
                { "hp", item.hp },
                { "maxHp", item.maxHp },
                { "attackMin", item.attackMin },
                { "attackMax", item.attackMax },
                { "attackRange", item.attackRange },
                { "attackSpeed", item.attackSpeed },
                { "speed", item.speed },
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
        j.at("id").get_to(item.id);
        j.at("type").get_to(item.type);
        j.at("count").get_to(item.count);
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
                { "id", item.id },
                { "type", item.type },
                { "count", item.count },
                { "attack", item.attack },
                { "defence", item.defence },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
            };
    }

    void from_json(const nlohmann::json& j, PlayerEquipment& item)
    {
        j.at("armor").get_to(item.armor);
        j.at("gloves").get_to(item.gloves);
        j.at("shoes").get_to(item.shoes);
        j.at("weapon").get_to(item.weapon);
    }

    void to_json(nlohmann::json& j, const PlayerEquipment& item)
    {
        j = nlohmann::json
            {
                { "armor", item.armor },
                { "gloves", item.gloves },
                { "shoes", item.shoes },
                { "weapon", item.weapon },
            };
    }

    void from_json(const nlohmann::json& j, PlayerQuickSlot& item)
    {
        j.at("index").get_to(item.index);
        j.at("type").get_to(item.type);
        j.at("id").get_to(item.id);
    }

    void to_json(nlohmann::json& j, const PlayerQuickSlot& item)
    {
        j = nlohmann::json
            {
                { "index", item.index },
                { "type", item.type },
                { "id", item.id },
            };
    }

    void from_json(const nlohmann::json& j, PlayerSkill& item)
    {
        j.at("id").get_to(item.id);
    }

    void to_json(nlohmann::json& j, const PlayerSkill& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
            };
    }

    void from_json(const nlohmann::json& j, PlayerInventoryItem& item)
    {
        j.at("slot").get_to(item.slot);
        j.at("id").get_to(item.id);
        j.at("count").get_to(item.count);
        j.at("attack").get_to(item.attack);
        j.at("defence").get_to(item.defence);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
    }

    void to_json(nlohmann::json& j, const PlayerInventoryItem& item)
    {
        j = nlohmann::json
            {
                { "slot", item.slot },
                { "id", item.id },
                { "count", item.count },
                { "attack", item.attack },
                { "defence", item.defence },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
            };
    }

    void from_json(const nlohmann::json& j, RemotePlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("transform").get_to(item.transform);
        j.at("base").get_to(item.base);
        j.at("equipment").get_to(item.equipment);
    }

    void to_json(nlohmann::json& j, const RemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "transform", item.transform },
                { "base", item.base },
                { "equipment", item.equipment },
            };
    }

    void from_json(const nlohmann::json& j, Player& item)
    {
        j.at("id").get_to(item.id);
        j.at("transform").get_to(item.transform);
        j.at("base").get_to(item.base);
        j.at("equipment").get_to(item.equipment);
        j.at("gold").get_to(item.gold);
        j.at("itemsCount").get_to(item.itemsCount);
        j.at("items").get_to(item.items);
        j.at("quickSlotCount").get_to(item.quickSlotCount);
        j.at("quickSlots").get_to(item.quickSlots);
        j.at("skillsCount").get_to(item.skillsCount);
        j.at("skills").get_to(item.skills);
        j.at("exp").get_to(item.exp);
        j.at("expMax").get_to(item.expMax);
    }

    void to_json(nlohmann::json& j, const Player& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "transform", item.transform },
                { "base", item.base },
                { "equipment", item.equipment },
                { "gold", item.gold },
                { "itemsCount", item.itemsCount },
                { "items", item.items },
                { "quickSlotCount", item.quickSlotCount },
                { "quickSlots", item.quickSlots },
                { "skillsCount", item.skillsCount },
                { "skills", item.skills },
                { "exp", item.exp },
                { "expMax", item.expMax },
            };
    }

    void from_json(const nlohmann::json& j, Monster& item)
    {
        j.at("dataId").get_to(item.dataId);
        j.at("id").get_to(item.id);
        j.at("transform").get_to(item.transform);
        j.at("hp").get_to(item.hp);
        j.at("maxHp").get_to(item.maxHp);
        j.at("attackMin").get_to(item.attackMin);
        j.at("attackMax").get_to(item.attackMax);
        j.at("attackRange").get_to(item.attackRange);
        j.at("attackSpeed").get_to(item.attackSpeed);
        j.at("speed").get_to(item.speed);
        j.at("defence").get_to(item.defence);
    }

    void to_json(nlohmann::json& j, const Monster& item)
    {
        j = nlohmann::json
            {
                { "dataId", item.dataId },
                { "id", item.id },
                { "transform", item.transform },
                { "hp", item.hp },
                { "maxHp", item.maxHp },
                { "attackMin", item.attackMin },
                { "attackMax", item.attackMax },
                { "attackRange", item.attackRange },
                { "attackSpeed", item.attackSpeed },
                { "speed", item.speed },
                { "defence", item.defence },
            };
    }

}
