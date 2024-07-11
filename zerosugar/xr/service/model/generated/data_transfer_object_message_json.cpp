#include "data_transfer_object_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, DTOAccount& item)
    {
        j.at("accountId").get_to(item.accountId);
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
        j.at("gmLevel").get_to(item.gmLevel);
        j.at("banned").get_to(item.banned);
        j.at("deleted").get_to(item.deleted);
    }

    void to_json(nlohmann::json& j, const DTOAccount& item)
    {
        j = nlohmann::json
            {
                { "accountId", item.accountId },
                { "account", item.account },
                { "password", item.password },
                { "gmLevel", item.gmLevel },
                { "banned", item.banned },
                { "deleted", item.deleted },
            };
    }

    void from_json(const nlohmann::json& j, DTOItem& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("itemDataId").get_to(item.itemDataId);
        j.at("quantity").get_to(item.quantity);
        if (const auto iter = j.find("slot"); iter != j.end())
        {
            item.slot.emplace(*iter);
        }
        if (const auto iter = j.find("attack"); iter != j.end())
        {
            item.attack.emplace(*iter);
        }
        if (const auto iter = j.find("defence"); iter != j.end())
        {
            item.defence.emplace(*iter);
        }
        if (const auto iter = j.find("str"); iter != j.end())
        {
            item.str.emplace(*iter);
        }
        if (const auto iter = j.find("dex"); iter != j.end())
        {
            item.dex.emplace(*iter);
        }
        if (const auto iter = j.find("intell"); iter != j.end())
        {
            item.intell.emplace(*iter);
        }
    }

    void to_json(nlohmann::json& j, const DTOItem& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "itemDataId", item.itemDataId },
                { "quantity", item.quantity },
            };

        if (item.slot.has_value())
        {
            j["slot"] = *item.slot;
        }
        if (item.attack.has_value())
        {
            j["attack"] = *item.attack;
        }
        if (item.defence.has_value())
        {
            j["defence"] = *item.defence;
        }
        if (item.str.has_value())
        {
            j["str"] = *item.str;
        }
        if (item.dex.has_value())
        {
            j["dex"] = *item.dex;
        }
        if (item.intell.has_value())
        {
            j["intell"] = *item.intell;
        }
    }

    void from_json(const nlohmann::json& j, DTOEquipment& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("equipPosition").get_to(item.equipPosition);
    }

    void to_json(nlohmann::json& j, const DTOEquipment& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "equipPosition", item.equipPosition },
            };
    }

    void from_json(const nlohmann::json& j, DTOEquipItem& item)
    {
        j.at("item").get_to(item.item);
        j.at("equipPosition").get_to(item.equipPosition);
    }

    void to_json(nlohmann::json& j, const DTOEquipItem& item)
    {
        j = nlohmann::json
            {
                { "item", item.item },
                { "equipPosition", item.equipPosition },
            };
    }

    void from_json(const nlohmann::json& j, DTOCharacterAdd& item)
    {
        j.at("accountId").get_to(item.accountId);
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
    }

    void to_json(nlohmann::json& j, const DTOCharacterAdd& item)
    {
        j = nlohmann::json
            {
                { "accountId", item.accountId },
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
            };
    }

    void from_json(const nlohmann::json& j, DTOLobbyItem& item)
    {
        j.at("itemDataId").get_to(item.itemDataId);
        j.at("equipPosition").get_to(item.equipPosition);
    }

    void to_json(nlohmann::json& j, const DTOLobbyItem& item)
    {
        j = nlohmann::json
            {
                { "itemDataId", item.itemDataId },
                { "equipPosition", item.equipPosition },
            };
    }

    void from_json(const nlohmann::json& j, DTOLobbyCharacter& item)
    {
        j.at("characterId").get_to(item.characterId);
        j.at("slot").get_to(item.slot);
        j.at("name").get_to(item.name);
        j.at("level").get_to(item.level);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
        j.at("job").get_to(item.job);
        j.at("faceId").get_to(item.faceId);
        j.at("hairId").get_to(item.hairId);
        j.at("zoneId").get_to(item.zoneId);
        j.at("items").get_to(item.items);
    }

    void to_json(nlohmann::json& j, const DTOLobbyCharacter& item)
    {
        j = nlohmann::json
            {
                { "characterId", item.characterId },
                { "slot", item.slot },
                { "name", item.name },
                { "level", item.level },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
                { "job", item.job },
                { "faceId", item.faceId },
                { "hairId", item.hairId },
                { "zoneId", item.zoneId },
                { "items", item.items },
            };
    }

    void from_json(const nlohmann::json& j, DTOCharacter& item)
    {
        j.at("characterId").get_to(item.characterId);
        j.at("name").get_to(item.name);
        j.at("level").get_to(item.level);
        j.at("str").get_to(item.str);
        j.at("dex").get_to(item.dex);
        j.at("intell").get_to(item.intell);
        j.at("job").get_to(item.job);
        j.at("faceId").get_to(item.faceId);
        j.at("hairId").get_to(item.hairId);
        j.at("zoneId").get_to(item.zoneId);
        j.at("items").get_to(item.items);
        j.at("equipments").get_to(item.equipments);
    }

    void to_json(nlohmann::json& j, const DTOCharacter& item)
    {
        j = nlohmann::json
            {
                { "characterId", item.characterId },
                { "name", item.name },
                { "level", item.level },
                { "str", item.str },
                { "dex", item.dex },
                { "intell", item.intell },
                { "job", item.job },
                { "faceId", item.faceId },
                { "hairId", item.hairId },
                { "zoneId", item.zoneId },
                { "items", item.items },
                { "equipments", item.equipments },
            };
    }

}
