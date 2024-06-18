#include "coordination_channel_input_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::coordination::input
{
    void Authenticate::Deserialize(PacketReader& reader)
    {
        serverId = reader.Read<int64_t>();
    }

    void Authenticate::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(serverId);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case Authenticate::opcode:
            {
                auto item = std::make_unique<Authenticate>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
