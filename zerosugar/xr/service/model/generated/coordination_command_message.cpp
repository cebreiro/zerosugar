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
        }
        return {};
    }
}
