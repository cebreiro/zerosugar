#include "coordination_command_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::coordination::command
{
    void LaunchGameInstance::Deserialize(PacketReader& reader)
    {
        gameInstanceId = reader.Read<int64_t>();
        zoneId = reader.Read<int32_t>();
    }

    void LaunchGameInstance::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(gameInstanceId);
        writer.Write<int32_t>(zoneId);
    }

    void BroadcastChatting::Deserialize(PacketReader& reader)
    {
        message = reader.ReadString();
    }

    void BroadcastChatting::Serialize(PacketWriter& writer) const
    {
        writer.Write(message);
    }

    void NotifyDungeonMatchGroupCreation::Deserialize(PacketReader& reader)
    {
        userId = reader.Read<int64_t>();
    }

    void NotifyDungeonMatchGroupCreation::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(userId);
    }

    void NotifyDungeonMatchGroupApproved::Deserialize(PacketReader& reader)
    {
        userId = reader.Read<int64_t>();
        ip = reader.ReadString();
        port = reader.Read<int32_t>();
    }

    void NotifyDungeonMatchGroupApproved::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(userId);
        writer.Write(ip);
        writer.Write<int32_t>(port);
    }

    void NotifyDungeonMatchGroupRejected::Deserialize(PacketReader& reader)
    {
        userId = reader.Read<int64_t>();
    }

    void NotifyDungeonMatchGroupRejected::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(userId);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case LaunchGameInstance::opcode:
            {
                auto item = std::make_unique<LaunchGameInstance>();
                item->Deserialize(reader);

                return item;
            }
            case BroadcastChatting::opcode:
            {
                auto item = std::make_unique<BroadcastChatting>();
                item->Deserialize(reader);

                return item;
            }
            case NotifyDungeonMatchGroupCreation::opcode:
            {
                auto item = std::make_unique<NotifyDungeonMatchGroupCreation>();
                item->Deserialize(reader);

                return item;
            }
            case NotifyDungeonMatchGroupApproved::opcode:
            {
                auto item = std::make_unique<NotifyDungeonMatchGroupApproved>();
                item->Deserialize(reader);

                return item;
            }
            case NotifyDungeonMatchGroupRejected::opcode:
            {
                auto item = std::make_unique<NotifyDungeonMatchGroupRejected>();
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
            case LaunchGameInstance::opcode:
            {
                LaunchGameInstance item;
                item.Deserialize(reader);

                return item;
            }
            case BroadcastChatting::opcode:
            {
                BroadcastChatting item;
                item.Deserialize(reader);

                return item;
            }
            case NotifyDungeonMatchGroupCreation::opcode:
            {
                NotifyDungeonMatchGroupCreation item;
                item.Deserialize(reader);

                return item;
            }
            case NotifyDungeonMatchGroupApproved::opcode:
            {
                NotifyDungeonMatchGroupApproved item;
                item.Deserialize(reader);

                return item;
            }
            case NotifyDungeonMatchGroupRejected::opcode:
            {
                NotifyDungeonMatchGroupRejected item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
