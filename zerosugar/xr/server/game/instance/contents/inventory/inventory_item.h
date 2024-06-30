#pragma once
#include "zerosugar/xr/server/game/instance/contents/inventory/item_id_type.h"

namespace zerosugar::xr
{
    enum class InventoryItemSlotType
    {
        Inventory,
        Equipment,
    };

    struct InventoryItem
    {
        game_item_id_type itemId = game_item_id_type::Default();
        int32_t itemDataId = 0;
        int32_t quantity = 0;

        int32_t slot = {};
        InventoryItemSlotType slotType = InventoryItemSlotType::Inventory;

        std::optional<int32_t> attack = {};
        std::optional<int32_t> defence = {};
        std::optional<int32_t> str = {};
        std::optional<int32_t> dex = {};
        std::optional<int32_t> intell = {};
    };
}
