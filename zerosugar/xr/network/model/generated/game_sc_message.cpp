#include "game_sc_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::game::sc
{
    void EnterGame::Deserialize(PacketReader& reader)
    {
        zoneId = reader.Read<int32_t>();
        remotePlayersCount = reader.Read<int32_t>();
        for (int32_t i = 0; i < remotePlayersCount; ++i)
        {
            remotePlayers.emplace_back(reader.Read<RemotePlayer>());
        }
        monstersCount = reader.Read<int32_t>();
        for (int32_t i = 0; i < monstersCount; ++i)
        {
            monsters.emplace_back(reader.Read<Monster>());
        }
        localPlayer = reader.Read<Player>();
    }

    void EnterGame::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(zoneId);
        writer.Write<int32_t>(remotePlayersCount);
        for (const auto& item : remotePlayers)
        {
            writer.WriteObject(item);
        }
        writer.Write<int32_t>(monstersCount);
        for (const auto& item : monsters)
        {
            writer.WriteObject(item);
        }
        writer.Write(localPlayer);
    }

    void AddRemotePlayer::Deserialize(PacketReader& reader)
    {
        player = reader.Read<RemotePlayer>();
    }

    void AddRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write(player);
    }

    void RemoveRemotePlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
    }

    void RemoveRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
    }

    void MoveRemotePlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        position = reader.Read<Position>();
        rotation = reader.Read<Rotation>();
    }

    void MoveRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(position);
        writer.Write(rotation);
    }

    void StopRemotePlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        position = reader.Read<Position>();
    }

    void StopRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(position);
    }

    void SprintRemotePlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
    }

    void SprintRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
    }

    void RollDodgeRemotePlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        rotation = reader.Read<Rotation>();
    }

    void RollDodgeRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(rotation);
    }

    void NotifyChattingMessage::Deserialize(PacketReader& reader)
    {
        type = reader.Read<int32_t>();
        message = reader.ReadString();
    }

    void NotifyChattingMessage::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(type);
        writer.Write(message);
    }

    void AddInventory::Deserialize(PacketReader& reader)
    {
        itemsCount = reader.Read<int32_t>();
        for (int32_t i = 0; i < itemsCount; ++i)
        {
            items.emplace_back(reader.Read<PlayerInventoryItem>());
        }
    }

    void AddInventory::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(itemsCount);
        for (const auto& item : items)
        {
            writer.WriteObject(item);
        }
    }

    void RemoveInventory::Deserialize(PacketReader& reader)
    {
        slotsCount = reader.Read<int32_t>();
        for (int32_t i = 0; i < slotsCount; ++i)
        {
            slots.emplace_back(reader.Read<int32_t>());
        }
    }

    void RemoveInventory::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(slotsCount);
        for (const auto& item : slots)
        {
            writer.WriteObject(item);
        }
    }

    void NotifySwapItemResult::Deserialize(PacketReader& reader)
    {
        srcEquipment = reader.Read<bool>();
        srcHasItem = reader.Read<bool>();
        srcItem = reader.Read<PlayerInventoryItem>();
        destEquipment = reader.Read<bool>();
        destHasItem = reader.Read<bool>();
        destItem = reader.Read<PlayerInventoryItem>();
    }

    void NotifySwapItemResult::Serialize(PacketWriter& writer) const
    {
        writer.Write<bool>(srcEquipment);
        writer.Write<bool>(srcHasItem);
        writer.Write(srcItem);
        writer.Write<bool>(destEquipment);
        writer.Write<bool>(destHasItem);
        writer.Write(destItem);
    }

    void ChangeRemotePlayerEquipItem::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        equipPosition = reader.Read<int32_t>();
        itemId = reader.Read<int32_t>();
    }

    void ChangeRemotePlayerEquipItem::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write<int32_t>(equipPosition);
        writer.Write<int32_t>(itemId);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case EnterGame::opcode:
            {
                auto item = std::make_unique<EnterGame>();
                item->Deserialize(reader);

                return item;
            }
            case AddRemotePlayer::opcode:
            {
                auto item = std::make_unique<AddRemotePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case RemoveRemotePlayer::opcode:
            {
                auto item = std::make_unique<RemoveRemotePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case MoveRemotePlayer::opcode:
            {
                auto item = std::make_unique<MoveRemotePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case StopRemotePlayer::opcode:
            {
                auto item = std::make_unique<StopRemotePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case SprintRemotePlayer::opcode:
            {
                auto item = std::make_unique<SprintRemotePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case RollDodgeRemotePlayer::opcode:
            {
                auto item = std::make_unique<RollDodgeRemotePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case NotifyChattingMessage::opcode:
            {
                auto item = std::make_unique<NotifyChattingMessage>();
                item->Deserialize(reader);

                return item;
            }
            case AddInventory::opcode:
            {
                auto item = std::make_unique<AddInventory>();
                item->Deserialize(reader);

                return item;
            }
            case RemoveInventory::opcode:
            {
                auto item = std::make_unique<RemoveInventory>();
                item->Deserialize(reader);

                return item;
            }
            case NotifySwapItemResult::opcode:
            {
                auto item = std::make_unique<NotifySwapItemResult>();
                item->Deserialize(reader);

                return item;
            }
            case ChangeRemotePlayerEquipItem::opcode:
            {
                auto item = std::make_unique<ChangeRemotePlayerEquipItem>();
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
            case EnterGame::opcode:
            {
                EnterGame item;
                item.Deserialize(reader);

                return item;
            }
            case AddRemotePlayer::opcode:
            {
                AddRemotePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case RemoveRemotePlayer::opcode:
            {
                RemoveRemotePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case MoveRemotePlayer::opcode:
            {
                MoveRemotePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case StopRemotePlayer::opcode:
            {
                StopRemotePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case SprintRemotePlayer::opcode:
            {
                SprintRemotePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case RollDodgeRemotePlayer::opcode:
            {
                RollDodgeRemotePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case NotifyChattingMessage::opcode:
            {
                NotifyChattingMessage item;
                item.Deserialize(reader);

                return item;
            }
            case AddInventory::opcode:
            {
                AddInventory item;
                item.Deserialize(reader);

                return item;
            }
            case RemoveInventory::opcode:
            {
                RemoveInventory item;
                item.Deserialize(reader);

                return item;
            }
            case NotifySwapItemResult::opcode:
            {
                NotifySwapItemResult item;
                item.Deserialize(reader);

                return item;
            }
            case ChangeRemotePlayerEquipItem::opcode:
            {
                ChangeRemotePlayerEquipItem item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
