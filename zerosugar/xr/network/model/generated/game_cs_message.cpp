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

    void PlayerMove::Deserialize(PacketReader& reader)
    {
        position = reader.Read<Position>();
    }

    void PlayerMove::Serialize(PacketWriter& writer) const
    {
        writer.Write(position);
    }

    void PlayerStopMovement::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        position = reader.Read<Position>();
    }

    void PlayerStopMovement::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(position);
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
            case PlayerMove::opcode:
            {
                auto item = std::make_unique<PlayerMove>();
                item->Deserialize(reader);

                return item;
            }
            case PlayerStopMovement::opcode:
            {
                auto item = std::make_unique<PlayerStopMovement>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
