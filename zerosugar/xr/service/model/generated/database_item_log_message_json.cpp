#include "database_item_log_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, EquipItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("equipPosition").get_to(item.equipPosition);
    }

    void to_json(nlohmann::json& j, const EquipItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "equipPosition", item.equipPosition },
            };
    }

    void from_json(const nlohmann::json& j, UnequipItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("invenotrySlot").get_to(item.invenotrySlot);
    }

    void to_json(nlohmann::json& j, const UnequipItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "invenotrySlot", item.invenotrySlot },
            };
    }

    void from_json(const nlohmann::json& j, ShiftItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("invenotrySlot").get_to(item.invenotrySlot);
    }

    void to_json(nlohmann::json& j, const ShiftItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "invenotrySlot", item.invenotrySlot },
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

    void from_json(const nlohmann::json& j, UseItemLog& item)
    {
        j.at("itemId").get_to(item.itemId);
        j.at("quantity").get_to(item.quantity);
    }

    void to_json(nlohmann::json& j, const UseItemLog& item)
    {
        j = nlohmann::json
            {
                { "itemId", item.itemId },
                { "quantity", item.quantity },
            };
    }

}
