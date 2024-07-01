#include "database_item_log_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, EquipItemLog& item)
    {
        j.at("characterId").get_to(item.characterId);
        j.at("itemId").get_to(item.itemId);
        j.at("equipPosition").get_to(item.equipPosition);
    }

    void to_json(nlohmann::json& j, const EquipItemLog& item)
    {
        j = nlohmann::json
            {
                { "characterId", item.characterId },
                { "itemId", item.itemId },
                { "equipPosition", item.equipPosition },
            };
    }

    void from_json(const nlohmann::json& j, UnequipItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("inventorySlot").get_to(item.inventorySlot);
    }

    void to_json(nlohmann::json& j, const UnequipItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "inventorySlot", item.inventorySlot },
            };
    }

    void from_json(const nlohmann::json& j, ShiftItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("inventorySlot").get_to(item.inventorySlot);
    }

    void to_json(nlohmann::json& j, const ShiftItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "inventorySlot", item.inventorySlot },
            };
    }

    void from_json(const nlohmann::json& j, DiscardItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
    }

    void to_json(nlohmann::json& j, const DiscardItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
            };
    }

    void from_json(const nlohmann::json& j, ChangeItemQuantityLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("quantity").get_to(item.quantity);
    }

    void to_json(nlohmann::json& j, const ChangeItemQuantityLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "quantity", item.quantity },
            };
    }

}
