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
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const MoveRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "position", item.position },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, StopRemotePlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("position").get_to(item.position);
    }

    void to_json(nlohmann::json& j, const StopRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "position", item.position },
            };
    }

    void from_json(const nlohmann::json& j, SprintRemotePlayer& item)
    {
        j.at("id").get_to(item.id);
    }

    void to_json(nlohmann::json& j, const SprintRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
            };
    }

    void from_json(const nlohmann::json& j, RollDodgeRemotePlayer& item)
    {
        j.at("id").get_to(item.id);
        j.at("rotation").get_to(item.rotation);
    }

    void to_json(nlohmann::json& j, const RollDodgeRemotePlayer& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "rotation", item.rotation },
            };
    }

    void from_json(const nlohmann::json& j, NotifyChattingMessage& item)
    {
        j.at("type").get_to(item.type);
        j.at("message").get_to(item.message);
    }

    void to_json(nlohmann::json& j, const NotifyChattingMessage& item)
    {
        j = nlohmann::json
            {
                { "type", item.type },
                { "message", item.message },
            };
    }

    void from_json(const nlohmann::json& j, AddInventory& item)
    {
        j.at("itemsCount").get_to(item.itemsCount);
        j.at("items").get_to(item.items);
    }

    void to_json(nlohmann::json& j, const AddInventory& item)
    {
        j = nlohmann::json
            {
                { "itemsCount", item.itemsCount },
                { "items", item.items },
            };
    }

    void from_json(const nlohmann::json& j, RemoveInventory& item)
    {
        j.at("slotsCount").get_to(item.slotsCount);
        j.at("slots").get_to(item.slots);
    }

    void to_json(nlohmann::json& j, const RemoveInventory& item)
    {
        j = nlohmann::json
            {
                { "slotsCount", item.slotsCount },
                { "slots", item.slots },
            };
    }

    void from_json(const nlohmann::json& j, NotifySwapItemResult& item)
    {
        j.at("srcEquipment").get_to(item.srcEquipment);
        j.at("srcHasItem").get_to(item.srcHasItem);
        j.at("srcItem").get_to(item.srcItem);
        j.at("destEquipment").get_to(item.destEquipment);
        j.at("destHasItem").get_to(item.destHasItem);
        j.at("destItem").get_to(item.destItem);
    }

    void to_json(nlohmann::json& j, const NotifySwapItemResult& item)
    {
        j = nlohmann::json
            {
                { "srcEquipment", item.srcEquipment },
                { "srcHasItem", item.srcHasItem },
                { "srcItem", item.srcItem },
                { "destEquipment", item.destEquipment },
                { "destHasItem", item.destHasItem },
                { "destItem", item.destItem },
            };
    }

    void from_json(const nlohmann::json& j, ChangeRemotePlayerEquipItem& item)
    {
        j.at("id").get_to(item.id);
        j.at("equipPosition").get_to(item.equipPosition);
        j.at("itemId").get_to(item.itemId);
    }

    void to_json(nlohmann::json& j, const ChangeRemotePlayerEquipItem& item)
    {
        j = nlohmann::json
            {
                { "id", item.id },
                { "equipPosition", item.equipPosition },
                { "itemId", item.itemId },
            };
    }

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupCreation& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupCreation& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, NotifyDungeonMatchFailure& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const NotifyDungeonMatchFailure& item)
    {
        (void)j;
        (void)item;
    }

    void from_json(const nlohmann::json& j, NotifyDungeonMatchGroupApproved& item)
    {
        j.at("ip").get_to(item.ip);
        j.at("port").get_to(item.port);
    }

    void to_json(nlohmann::json& j, const NotifyDungeonMatchGroupApproved& item)
    {
        j = nlohmann::json
            {
                { "ip", item.ip },
                { "port", item.port },
            };
    }

}
