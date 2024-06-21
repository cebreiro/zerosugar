#include "game_cs_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::game::cs
{
    void Authenticate::Deserialize(PacketReader& reader)
    {
        authenticationToken = reader.ReadString();
    }

    void Authenticate::Serialize(PacketWriter& writer) const
    {
        writer.Write(authenticationToken);
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
