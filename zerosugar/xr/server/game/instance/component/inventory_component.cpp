#include "inventory_component.h"

namespace zerosugar::xr
{
    bool InventoryComponent::AddItem(const InventoryItem& item)
    {
        return _items.try_emplace(item.itemId, item).second;
    }

    bool InventoryComponent::Equip(data::EquipPosition position, int64_t itemId)
    {
        if (const auto iter = _items.find(itemId); iter != _items.end())
        {
            if (InventoryItem*& equipItemPtr = _equipItems[static_cast<int32_t>(position)]; !equipItemPtr)
            {
                equipItemPtr = &iter->second;

                return true;
            }
        }

        return false;
    }

    auto InventoryComponent::FindItem(int64_t itemId) const -> const InventoryItem*
    {
        const auto iter = _items.find(itemId);

        return iter != _items.end() ? &iter->second : nullptr;
    }

    auto InventoryComponent::GetEquipments() const -> std::array<const InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)>
    {
        std::array<const InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)> result;
        std::ranges::copy(_equipItems, result.begin());

        return result;
    }
}
