#pragma once
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/server/game/instance/game_constants.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"
#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_item.h"
#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_change_log.h"

namespace zerosugar::xr::service
{
    struct DTOItem;
    struct DTOEquipment;
}

namespace zerosugar::xr
{
    class InventoryComponent : public GameComponent
    {
    public:
        bool Initialize(const std::vector<service::DTOItem>& items, const std::vector<service::DTOEquipment>& equipments);

        bool HasEquipItem(game_item_id_type itemId) const;

        void ClearChangeLogs();

        bool Equip(data::EquipPosition destPosition, int32_t srcPosition);
        bool Unequip(data::EquipPosition srcPosition, int32_t destPosition);
        bool ShiftItem(int32_t srcPosition, int32_t destPosition);

        auto FindItem(game_item_id_type itemId) const -> const InventoryItem*;
        auto FindItemBySlot(InventoryItemSlotType type, int32_t slot) const -> const InventoryItem*;

        inline auto GetInventoryItemsRange() const;
        inline auto GetEquippedItemRange() const;

        auto GetEquipments() const -> std::array<const InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)>;
        auto GetChangeLogs() const -> const std::vector<inventory_change_log_type>&;

    private:
        bool AddInventory(InventoryItem& item);
        bool AddEquipment(InventoryItem& item);

        auto FindMutableItem(game_item_id_type itemId) -> InventoryItem*;

        void AddEquipLog(game_item_id_type itemId, data::EquipPosition position);
        void AddUnequipLog(game_item_id_type itemId, int32_t slot);
        void AddShiftItemLog(game_item_id_type itemId, int32_t slot);
        void AddDiscardItemLog(game_item_id_type itemId);
        void AddUseItemLog(game_item_id_type itemId, int32_t quantity);

        static bool IsValidSlot(int32_t slot);

    private:
        std::unordered_map<game_item_id_type, InventoryItem> _items;

        std::array<InventoryItem*, game_constant::inventory_size> _inventory = {};
        std::array<InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)> _equipments = {};

        std::vector<inventory_change_log_type> _itemLogs;
    };

    inline auto InventoryComponent::GetInventoryItemsRange() const
    {
        return _inventory
            | std::views::filter([](const InventoryItem* ptr)
                {
                    return ptr != nullptr;
                })
            | std::views::transform([](const InventoryItem* ptr) -> const InventoryItem&
                {
                    return *ptr;
                });
    }

    inline auto InventoryComponent::GetEquippedItemRange() const
    {
        return _equipments
            | std::views::filter([](const InventoryItem* item)
                {
                    return item != nullptr;
                })
            | std::views::transform([](const InventoryItem* item) -> const InventoryItem&
                {
                    return *item;
                });
    }
}
