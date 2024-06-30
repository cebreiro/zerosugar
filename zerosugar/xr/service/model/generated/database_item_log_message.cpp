#include "database_item_log_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::service
{
    void EquipItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        equipPosition = reader.Read<int32_t>();
    }

    void EquipItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(equipPosition);
    }

    void UnequipItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        invenotrySlot = reader.Read<int32_t>();
    }

    void UnequipItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(invenotrySlot);
    }

    void ShiftItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        invenotrySlot = reader.Read<int32_t>();
    }

    void ShiftItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
        writer.Write<int32_t>(invenotrySlot);
    }

    void DiscardItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
    }

    void DiscardItemLog::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(itemId);
    }

    void UseItemLog::Deserialize(PacketReader& reader)
    {
        itemId = reader.Read<int64_t>();
        quantity = reader.Read<int32_t>();
    }

    void UseItemLog::Serialize(PacketWriter& writer) const
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
            case DiscardItemLog::opcode:
            {
                auto item = std::make_unique<DiscardItemLog>();
                item->Deserialize(reader);

                return item;
            }
            case UseItemLog::opcode:
            {
                auto item = std::make_unique<UseItemLog>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
