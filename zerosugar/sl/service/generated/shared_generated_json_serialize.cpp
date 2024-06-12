#include "shared_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, AuthToken& item)
    {
        j.at("values").get_to(item.values);
    }

    void to_json(nlohmann::json& j, const AuthToken& item)
    {
        j = nlohmann::json
            {
                { "values", item.values },
            };
    }

    void from_json(const nlohmann::json& j, Account& item)
    {
        j.at("id").get_to(item.id);
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
        j.at("gmLevel").get_to(item.gmLevel);
        j.at("banned").get_to(item.banned);
        j.at("banReason").get_to(item.banReason);
    }

    void to_json(nlohmann::json& j, const Account& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "account", item.account },
                { "password", item.password },
                { "gmLevel", item.gmLevel },
                { "banned", item.banned },
                { "banReason", item.banReason },
            };
    }

    void from_json(const nlohmann::json& j, AccountUpdate& item)
    {
        j.at("id").get_to(item.id);
        if (const auto iter = j.find("password"); iter != j.end())
        {
            item.password.emplace(*iter);
        }
        if (const auto iter = j.find("gmLevel"); iter != j.end())
        {
            item.gmLevel.emplace(*iter);
        }
        if (const auto iter = j.find("banned"); iter != j.end())
        {
            item.banned.emplace(*iter);
        }
        if (const auto iter = j.find("banReason"); iter != j.end())
        {
            item.banReason.emplace(*iter);
        }
    }

    void to_json(nlohmann::json& j, const AccountUpdate& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
            };

        if (item.password.has_value())
        {
            j.push_back(nlohmann::json{ "password", *item.password });
        }
        if (item.gmLevel.has_value())
        {
            j.push_back(nlohmann::json{ "gmLevel", *item.gmLevel });
        }
        if (item.banned.has_value())
        {
            j.push_back(nlohmann::json{ "banned", *item.banned });
        }
        if (item.banReason.has_value())
        {
            j.push_back(nlohmann::json{ "banReason", *item.banReason });
        }
    }

    void from_json(const nlohmann::json& j, SlotPosition& item)
    {
        j.at("type").get_to(item.type);
        j.at("value1").get_to(item.value1);
        j.at("value2").get_to(item.value2);
        j.at("value3").get_to(item.value3);
    }

    void to_json(nlohmann::json& j, const SlotPosition& item)
    {
        j = nlohmann::json
            {
                { "type", item.type },
                { "value1", item.value1 },
                { "value2", item.value2 },
                { "value3", item.value3 },
            };
    }

    void from_json(const nlohmann::json& j, Item& item)
    {
        j.at("uniqueId").get_to(item.uniqueId);
        j.at("ownerId").get_to(item.ownerId);
        j.at("dataId").get_to(item.dataId);
        j.at("quantity").get_to(item.quantity);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const Item& item)
    {
        j = nlohmann::json
            {
                { "uniqueId", item.uniqueId },
                { "ownerId", item.ownerId },
                { "dataId", item.dataId },
                { "quantity", item.quantity },
                { "position", item.position },
            };
    }

    void from_json(const nlohmann::json& j, Skill& item)
    {
        j.at("uniqueId").get_to(item.uniqueId);
        j.at("ownerId").get_to(item.ownerId);
        j.at("jobId").get_to(item.jobId);
        j.at("skillId").get_to(item.skillId);
        j.at("level").get_to(item.level);
        j.at("cooldown").get_to(item.cooldown);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const Skill& item)
    {
        j = nlohmann::json
            {
                { "uniqueId", item.uniqueId },
                { "ownerId", item.ownerId },
                { "jobId", item.jobId },
                { "skillId", item.skillId },
                { "level", item.level },
                { "cooldown", item.cooldown },
                { "position", item.position },
            };
    }

    void from_json(const nlohmann::json& j, Job& item)
    {
        j.at("grade").get_to(item.grade);
        j.at("dataId").get_to(item.dataId);
        j.at("level").get_to(item.level);
        j.at("exp").get_to(item.exp);
        j.at("sp").get_to(item.sp);
    }

    void to_json(nlohmann::json& j, const Job& item)
    {
        j = nlohmann::json
            {
                { "grade", item.grade },
                { "dataId", item.dataId },
                { "level", item.level },
                { "exp", item.exp },
                { "sp", item.sp },
            };
    }

    void from_json(const nlohmann::json& j, CharacterStat& item)
    {
        j.at("gender").get_to(item.gender);
        j.at("hp").get_to(item.hp);
        j.at("mp").get_to(item.mp);
        j.at("level").get_to(item.level);
        j.at("exp").get_to(item.exp);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("accr").get_to(item.accr);
        j.at("health").get_to(item.health);
        j.at("intell").get_to(item.intell);
        j.at("wis").get_to(item.wis);
        j.at("will").get_to(item.will);
        j.at("statPoint").get_to(item.statPoint);
        j.at("elementalWater").get_to(item.elementalWater);
        j.at("elementalFire").get_to(item.elementalFire);
        j.at("elementalLightning").get_to(item.elementalLightning);
    }

    void to_json(nlohmann::json& j, const CharacterStat& item)
    {
        j = nlohmann::json
            {
                { "gender", item.gender },
                { "hp", item.hp },
                { "mp", item.mp },
                { "level", item.level },
                { "exp", item.exp },
                { "str", item.str },
                { "dex", item.dex },
                { "accr", item.accr },
                { "health", item.health },
                { "intell", item.intell },
                { "wis", item.wis },
                { "will", item.will },
                { "statPoint", item.statPoint },
                { "elementalWater", item.elementalWater },
                { "elementalFire", item.elementalFire },
                { "elementalLightning", item.elementalLightning },
            };
    }

    void from_json(const nlohmann::json& j, Character& item)
    {
        j.at("id").get_to(item.id);
        j.at("slot").get_to(item.slot);
        j.at("name").get_to(item.name);
        j.at("hairColor").get_to(item.hairColor);
        j.at("hair").get_to(item.hair);
        j.at("skinColor").get_to(item.skinColor);
        j.at("face").get_to(item.face);
        j.at("arms").get_to(item.arms);
        j.at("running").get_to(item.running);
        j.at("gold").get_to(item.gold);
        j.at("inventorySize").get_to(item.inventorySize);
        j.at("zone").get_to(item.zone);
        j.at("stage").get_to(item.stage);
        j.at("x").get_to(item.x);
        j.at("y").get_to(item.y);
        j.at("stat").get_to(item.stat);
        j.at("job").get_to(item.job);
        j.at("item").get_to(item.item);
        j.at("skill").get_to(item.skill);
    }

    void to_json(nlohmann::json& j, const Character& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "slot", item.slot },
                { "name", item.name },
                { "hairColor", item.hairColor },
                { "hair", item.hair },
                { "skinColor", item.skinColor },
                { "face", item.face },
                { "arms", item.arms },
                { "running", item.running },
                { "gold", item.gold },
                { "inventorySize", item.inventorySize },
                { "zone", item.zone },
                { "stage", item.stage },
                { "x", item.x },
                { "y", item.y },
                { "stat", item.stat },
                { "job", item.job },
                { "item", item.item },
                { "skill", item.skill },
            };
    }

}
