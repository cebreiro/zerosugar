#include "inventory_component.h"

#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"
#include "zerosugar/xr/service/model/generated/database_item_log_message_json.h"

namespace zerosugar::xr
{
    bool InventoryComponent::Initialize(int64_t cid, const std::vector<service::DTOItem>& items, const std::vector<service::DTOEquipment>& equipments)
    {
        _characterId = cid;

        bool success = true;

        for (const service::DTOItem& dto : items)
        {
            [[maybe_unused]]
            const auto& [iter, inserted] = _items.try_emplace(game_item_id_type(dto.itemId), InventoryItem{});
            success &= inserted;

            InventoryItem& item = iter->second;
            item.itemId = game_item_id_type(dto.itemId);
            item.itemDataId = dto.itemDataId;
            item.quantity = dto.quantity;
            item.slot = dto.slot.value_or(-1);

            item.attack = dto.attack;
            item.defence = dto.defence;
            item.str = dto.str;
            item.dex = dto.dex;
            item.intell = dto.intell;

            if (dto.slot.has_value())
            {
                success &= AddInventory(item);
            }
        }

        for (const service::DTOEquipment& equipment : equipments)
        {
            InventoryItem* item = FindMutableItem(game_item_id_type(equipment.itemId));
            if (!item)
            {
                success = false;
            }
            else
            {
                item->slot = equipment.equipPosition;
                item->slotType = InventoryItemSlotType::Equipment;

                success &= AddEquipment(*item);
            }
        }

        return success;
    }

    bool InventoryComponent::HasEquipItem(game_item_id_type itemId) const
    {
        return std::ranges::any_of(_equipments, [itemId](const InventoryItem* item)
            {
                return item && item->itemId == itemId;
            });
    }

    void InventoryComponent::ClearChangeLogs()
    {
        _itemLogs.clear();
    }

    bool InventoryComponent::CanStackItem(int32_t itemId, int32_t quantity, int32_t& resultQuantity) const
    {
        constexpr auto stackableItems = { 2000001 , 2000002, 2000003 };

        int32_t stackableSize = 0;

        const auto filter = [itemId](const InventoryItem* item)
            {
                return item && item->itemDataId == itemId;
            };

        for (const InventoryItem& inventoryItem : _inventory | std::views::filter(filter) | notnull::reference)
        {
            const int32_t diff = game_constant::stackable_item_max_quantity - inventoryItem.quantity;
            if (diff > 0)
            {
                stackableSize += diff;
            }

            if (stackableSize >= quantity)
            {
                break;
            }
        }

        resultQuantity = std::min(stackableSize, quantity);

        return stackableSize > 0;
    }

    bool InventoryComponent::HasEmptySlot() const
    {
        const auto iter = std::ranges::find_if(_inventory, [](const InventoryItem* item)
            {
                return !item;
            });

        return iter != _inventory.end();
    }

    bool InventoryComponent::StackItem(int32_t itemId, int32_t quantity)
    {
        std::vector<std::pair<PtrNotNull<InventoryItem>, int32_t>> changes;

        int32_t remain = quantity;

        const auto filter = [itemId](const InventoryItem* item)
            {
                return item && item->itemDataId == itemId;
            };

        for (InventoryItem& inventoryItem : _inventory | std::views::filter(filter) | notnull::reference)
        {
            if (remain <= 0)
            {
                break;
            }

            const int32_t diff = game_constant::stackable_item_max_quantity - inventoryItem.quantity;
            if (diff <= 0)
            {
                continue;
            }

            const int32_t addQuantity = std::min(diff, remain);
            remain -= addQuantity;

            changes.emplace_back(&inventoryItem, inventoryItem.quantity + addQuantity);
        }

        if (changes.empty() || remain > 0)
        {
            return false;
        }

        for (const auto& [inventoryItem, newQuantity] : changes)
        {
            inventoryItem->quantity = newQuantity;

            LogChangeItemQuantity(inventoryItem->itemId, newQuantity);
        }

        return true;
    }

    bool InventoryComponent::AddItem(int64_t itemUid, int32_t itemId, int32_t quantity)
    {
        const game_item_id_type id(itemUid);

        assert(!_items.contains(id));
        assert(quantity > 0);

        for (int32_t i = 0; i < std::ssize(_inventory); ++i)
        {
            InventoryItem*& inventoryItem = _inventory[i];
            if (inventoryItem)
            {
                continue;
            }

            InventoryItem& item = _items[id];
            item.itemDataId = itemId;
            item.itemId = id;
            item.slot = i;
            item.quantity = quantity;

            inventoryItem = &item;

            LogAddItem(id, itemId, quantity, i);

            return true;
        }

        return false;
    }

    bool InventoryComponent::RemoveItem(int32_t slot)
    {
        if (!IsValidSlot(slot))
        {
            return false;
        }

        InventoryItem*& item = _inventory[slot];
        if (!item)
        {
            return false;
        }

        const game_item_id_type id = item->itemId;

        [[maybe_unused]]
        const bool erased = _items.erase(id);
        assert(erased);

        item = nullptr;

        LogRemoveItem(id);

        return true;
    }

    bool InventoryComponent::Equip(data::EquipPosition destPosition, int32_t srcPosition)
    {
        if (!IsValid(destPosition) || !IsValidSlot(srcPosition))
        {
            return false;
        }

        InventoryItem* item = _inventory[srcPosition];
        if (!item)
        {
            return false;
        }

        InventoryItem* destPosItem = _equipments[static_cast<int32_t>(destPosition)];
        if (destPosItem)
        {
            LogEquip(item->itemId, destPosition);
            LogUnequip(destPosItem->itemId, srcPosition);

            std::swap(std::tie(item->slot, item->slotType), std::tie(destPosItem->slot, destPosItem->slotType));
        }
        else
        {
            LogEquip(item->itemId, destPosition);

            item->slot = static_cast<int32_t>(destPosition);
            item->slotType = InventoryItemSlotType::Equipment;
        }

        std::swap(_inventory[srcPosition], _equipments[static_cast<int32_t>(destPosition)]);

        assert(item == _equipments[static_cast<int32_t>(destPosition)]);
        assert(destPosItem == _inventory[srcPosition]);

        return true;
    }

    bool InventoryComponent::Unequip(data::EquipPosition srcPosition, int32_t destPosition)
    {
        if (!IsValid(srcPosition) || !IsValidSlot(destPosition))
        {
            return false;
        }

        InventoryItem* item = _equipments[static_cast<int32_t>(srcPosition)];
        if (!item)
        {
            return false;
        }

        InventoryItem* destPosItem = _inventory[destPosition];
        if (destPosItem)
        {
            LogUnequip(item->itemId, destPosition);
            LogEquip(destPosItem->itemId, srcPosition);

            std::swap(std::tie(destPosItem->slot, destPosItem->slotType), std::tie(item->slot, item->slotType));
        }
        else
        {
            LogUnequip(item->itemId, destPosition);

            item->slot = destPosition;
            item->slotType = InventoryItemSlotType::Inventory;
        }

        std::swap(_equipments[static_cast<int32_t>(srcPosition)], _inventory[destPosition]);

        assert(item == _inventory[ destPosition]);
        assert(destPosItem == _equipments[static_cast<int32_t>(srcPosition)]);

        return true;
    }

    bool InventoryComponent::ShiftItem(int32_t srcPosition, int32_t destPosition)
    {
        if (!IsValidSlot(srcPosition) || !IsValidSlot(destPosition))
        {
            return false;
        }

        InventoryItem* item = _inventory[srcPosition];
        if (!item)
        {
            return false;
        }

        InventoryItem* destPosItem = _inventory[destPosition];
        if (destPosItem)
        {
            LogShiftItem(item->itemId, destPosition);
            LogShiftItem(destPosItem->itemId, srcPosition);

            std::swap(std::tie(destPosItem->slot, destPosItem->slotType), std::tie(item->slot, item->slotType));
        }
        else
        {
            LogShiftItem(item->itemId, destPosition);

            item->slot = destPosition;
        }

        std::swap(_inventory[srcPosition], _inventory[destPosition]);

        return true;
    }

    auto InventoryComponent::FindItem(game_item_id_type itemId) const -> const InventoryItem*
    {
        const auto iter = _items.find(itemId);

        return iter != _items.end() ? &iter->second : nullptr;
    }

    auto InventoryComponent::FindItemBySlot(InventoryItemSlotType type, int32_t slot) const -> const InventoryItem*
    {
        switch (type)
        {
        case InventoryItemSlotType::Inventory:
        {
            if (!IsValidSlot(slot))
            {
                assert(false);

                return nullptr;
            }

            return _inventory[slot];
        }
        break;
        case InventoryItemSlotType::Equipment:
        {
            if (!IsValid(static_cast<data::EquipPosition>(slot)))
            {
                assert(false);

                return nullptr;
            }

            return _equipments[slot];
        }
        break;
        default:
            assert(false);
        }

        return nullptr;
    }

    auto InventoryComponent::GetCharacterId() const -> int64_t
    {
        return _characterId;
    }

    auto InventoryComponent::GetEquipments() const -> std::array<const InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)>
    {
        std::array<const InventoryItem*, static_cast<int32_t>(data::EquipPosition::Count)> result;
        std::ranges::copy(_equipments, result.begin());

        return result;
    }

    auto InventoryComponent::GetChangeLogs() const -> const std::vector<inventory_change_log_type>&
    {
        return _itemLogs;
    }

    bool InventoryComponent::AddInventory(InventoryItem& item)
    {
        if (!IsValidSlot(item.slot))
        {
            return false;
        }

        InventoryItem*& slot = _inventory[item.slot];
        if (slot)
        {
            return false;
        }

        slot = &item;

        return true;
    }

    bool InventoryComponent::AddEquipment(InventoryItem& item)
    {
        const auto position = static_cast<data::EquipPosition>(item.slot);

        if (!IsValid(position))
        {
            return false;
        }

        InventoryItem*& slot = _equipments[static_cast<int32_t>(position)];
        if (slot)
        {
            return false;
        }

        slot = &item;

        return true;
    }

    auto InventoryComponent::FindMutableItem(game_item_id_type itemId) -> InventoryItem*
    {
        const auto iter = _items.find(itemId);

        return iter != _items.end() ? &iter->second : nullptr;
    }

    void InventoryComponent::LogEquip(game_item_id_type itemId, data::EquipPosition position)
    {
        service::EquipItemLog log;
        log.characterId = _characterId;
        log.itemId = itemId.Unwrap();
        log.equipPosition = static_cast<int32_t>(position);

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::LogUnequip(game_item_id_type itemId, int32_t slot)
    {
        service::UnequipItemLog log;
        log.itemId = itemId.Unwrap();
        log.inventorySlot = slot;

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::LogShiftItem(game_item_id_type itemId, int32_t slot)
    {
        service::ShiftItemLog log;
        log.itemId = itemId.Unwrap();
        log.inventorySlot = slot;

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::LogAddItem(game_item_id_type itemId, int32_t dataId, int32_t quantity, int32_t slot)
    {
        service::AddItemLog log;
        log.itemId = itemId.Unwrap();
        log.characterId = _characterId;
        log.itemDataId = dataId;
        log.quantity = quantity;
        log.slot = slot;

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::LogRemoveItem(game_item_id_type itemId)
    {
        service::RemoveItemLog log;
        log.itemId = itemId.Unwrap();

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::LogChangeItemQuantity(game_item_id_type itemId, int32_t quantity)
    {
        service::ChangeItemQuantityLog log;
        log.itemId = itemId.Unwrap();
        log.quantity = quantity;

        _itemLogs.emplace_back(log);
    }

    bool InventoryComponent::IsValidSlot(int32_t slot)
    {
        return slot >= 0 && slot < game_constant::inventory_size;
    }
}
