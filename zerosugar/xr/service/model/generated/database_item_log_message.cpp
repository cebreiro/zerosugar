#include "database_item_log_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::service
{
    void EquipItemLog::Deserialize(PacketReader& reader)
    {
        characterId = reader.Read<int64_t>();
        itemId = reader.Read<int64_t>();
        equipPosition = reader.Read<int32_t>();
    }

    void EquipItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(characterId);
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(equipPosition);
    }

    void UnequipItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        inventorySlot = reader.Read<int32_t>();
    }

    void UnequipItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(inventorySlot);
    }

    void ShiftItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        inventorySlot = reader.Read<int32_t>();
    }

    void ShiftItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(inventorySlot);
    }

    void AddItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        characterId = reader.Read<int64_t>();
        itemDataId = reader.Read<int32_t>();
        quantity = reader.Read<int32_t>();
        slot = reader.Read<int32_t>();
    }

    void AddItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int64_t>(characterId);
        writer.Write<int32_t>(itemDataId);
        writer.Write<int32_t>(quantity);
        writer.Write<int32_t>(slot);
    }

    void RemoveItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
    }

    void RemoveItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
    }

    void ChangeItemQuantityLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        quantity = reader.Read<int32_t>();
    }

    void ChangeItemQuantityLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(quantity);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case EquipItemLog::opcode:
            {
                auto item = std::make_unique<EquipItemLog>();
                item->Deserialize(reader);

                return item;
            }
            case UnequipItemLog::opcode:
            {
                auto item = std::make_unique<UnequipItemLog>();
                item->Deserialize(reader);

                return item;
            }
            case ShiftItemLog::opcode:
            {
                auto item = std::make_unique<ShiftItemLog>();
                item->Deserialize(reader);

                return item;
            }
            case AddItemLog::opcode:
            {
                auto item = std::make_unique<AddItemLog>();
                item->Deserialize(reader);

                return item;
            }
            case RemoveItemLog::opcode:
            {
                auto item = std::make_unique<RemoveItemLog>();
                item->Deserialize(reader);

                return item;
            }
            case ChangeItemQuantityLog::opcode:
            {
                auto item = std::make_unique<ChangeItemQuantityLog>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto CreateAnyFrom(PacketReader& reader) -> std::any
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case EquipItemLog::opcode:
            {
                EquipItemLog item;
                item.Deserialize(reader);

                return item;
            }
            case UnequipItemLog::opcode:
            {
                UnequipItemLog item;
                item.Deserialize(reader);

                return item;
            }
            case ShiftItemLog::opcode:
            {
                ShiftItemLog item;
                item.Deserialize(reader);

                return item;
            }
            case AddItemLog::opcode:
            {
                AddItemLog item;
                item.Deserialize(reader);

                return item;
            }
            case RemoveItemLog::opcode:
            {
                RemoveItemLog item;
                item.Deserialize(reader);

                return item;
            }
            case ChangeItemQuantityLog::opcode:
            {
                ChangeItemQuantityLog item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&
    {
        switch(opcode)
        {
            case EquipItemLog::opcode:
            {
                return typeid(EquipItemLog);
            }
            case UnequipItemLog::opcode:
            {
                return typeid(UnequipItemLog);
            }
            case ShiftItemLog::opcode:
            {
                return typeid(ShiftItemLog);
            }
            case AddItemLog::opcode:
            {
                return typeid(AddItemLog);
            }
            case RemoveItemLog::opcode:
            {
                return typeid(RemoveItemLog);
            }
            case ChangeItemQuantityLog::opcode:
            {
                return typeid(ChangeItemQuantityLog);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
