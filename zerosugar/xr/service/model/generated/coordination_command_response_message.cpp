#include "coordination_command_response_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::coordination::command::response
{
    void Exception::Deserialize(PacketReader& reader)
    {
        message = reader.ReadString();
    }

    void Exception::Serialize(PacketWriter& writer) const
    {
        writer.Write(message);
    }

    void Authenticate::Deserialize(PacketReader& reader)
    {
        serverId = reader.Read<int64_t>();
    }

    void Authenticate::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(serverId);
    }

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

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case Exception::opcode:
            {
                auto item = std::make_unique<Exception>();
                item->Deserialize(reader);

                return item;
            }
            case Authenticate::opcode:
            {
                auto item = std::make_unique<Authenticate>();
                item->Deserialize(reader);

                return item;
            }
            case LaunchGameInstance::opcode:
            {
                auto item = std::make_unique<LaunchGameInstance>();
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
            case Exception::opcode:
            {
                Exception item;
                item.Deserialize(reader);

                return item;
            }
            case Authenticate::opcode:
            {
                Authenticate item;
                item.Deserialize(reader);

                return item;
            }
            case LaunchGameInstance::opcode:
            {
                LaunchGameInstance item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
