#pragma once
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"

namespace zerosugar::xr
{
    struct InventoryItem
    {
        int64_t itemId = 0;
        int32_t itemDataId = 0;
        int32_t quantity = 0;

        std::optional<int32_t> attack = {};
        std::optional<int32_t> defence = {};
        std::optional<int32_t> str = {};
        std::optional<int32_t> dex = {};
        std::optional<int32_t> intell = {};
    };

    class InventoryComponent : public GameComponent
    {
    public:
        bool AddItem(const InventoryItem& item);
        bool Equip(data::EquipPosition position, int64_t itemId);

        auto FindItem(int64_t itemId) const -> const InventoryItem*;

        inline auto GetInventoryItems() const;
        auto GetEquipments() const -> std::array<const InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)>;

    private:
        std::unordered_map<int64_t, InventoryItem> _items;
        std::array<InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)> _equipItems = {};
    };

    inline auto InventoryComponent::GetInventoryItems() const
    {
        return _items | std::views::values;
    }
}
