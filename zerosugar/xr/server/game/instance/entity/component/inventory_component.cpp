#include "inventory_component.h"

#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"
#include "zerosugar/xr/service/model/generated/database_item_log_message_json.h"

namespace zerosugar::xr
{
    bool InventoryComponent::Initialize(const std::vector<service::DTOItem>& items, const std::vector<service::DTOEquipment>& equipments)
    {
        bool success = true;

        for (const service::DTOItem& dto : items)
        {
            InventoryItem item;
            item.itemId = game_item_id_type(dto.itemId);
            item.itemDataId = dto.itemDataId;
            item.quantity = dto.quantity;
            item.slot = dto.slot.value_or(-1);

            item.attack = dto.attack;
            item.defence = dto.defence;
            item.str = dto.str;
            item.dex = dto.dex;
            item.intell = dto.intell;

            success &= _items.try_emplace(item.itemId, item).second;

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
            AddEquipLog(item->itemId, destPosition);
            AddUnequipLog(destPosItem->itemId, srcPosition);

            std::swap(std::tie(item->slot, item->slotType), std::tie(destPosItem->slot, destPosItem->slotType));
        }
        else
        {
            AddEquipLog(item->itemId, destPosition);

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
            AddUnequipLog(item->itemId, destPosition);
            AddEquipLog(destPosItem->itemId, srcPosition);

            std::swap(std::tie(destPosItem->slot, destPosItem->slotType), std::tie(item->slot, item->slotType));
        }
        else
        {
            AddUnequipLog(item->itemId, destPosition);

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
            AddShiftItemLog(item->itemId, destPosition);
            AddShiftItemLog(destPosItem->itemId, srcPosition);

            std::swap(std::tie(destPosItem->slot, destPosItem->slotType), std::tie(item->slot, item->slotType));
        }
        else
        {
            AddShiftItemLog(item->itemId, destPosition);

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

    void InventoryComponent::AddEquipLog(game_item_id_type itemId, data::EquipPosition position)
    {
        service::EquipItemLog log;
        log.itemId = itemId.Unwrap();
        log.equipPosition = static_cast<int32_t>(position);

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::AddUnequipLog(game_item_id_type itemId, int32_t slot)
    {
        service::UnequipItemLog log;
        log.itemId = itemId.Unwrap();
        log.invenotrySlot = slot;

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::AddShiftItemLog(game_item_id_type itemId, int32_t slot)
    {
        service::ShiftItemLog log;
        log.itemId = itemId.Unwrap();
        log.invenotrySlot = slot;

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::AddDiscardItemLog(game_item_id_type itemId)
    {
        service::DiscardItemLog log;
        log.itemId = itemId.Unwrap();

        _itemLogs.emplace_back(log);
    }

    void InventoryComponent::AddUseItemLog(game_item_id_type itemId, int32_t quantity)
    {
        service::UseItemLog log;
        log.itemId = itemId.Unwrap();
        log.quantity = quantity;

        _itemLogs.emplace_back(log);
    }

    bool InventoryComponent::IsValidSlot(int32_t slot)
    {
        return slot >= 0 && slot < game_constant::inventory_size;
    }
}
