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

    void MovePlayer::Deserialize(PacketReader& reader)
    {
        position = reader.Read<Position>();
    }

    void MovePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write(position);
    }

    void StopPlayerMovement::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        position = reader.Read<Position>();
    }

    void StopPlayerMovement::Serialize(PacketWriter& writer) const
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
            case MovePlayer::opcode:
            {
                auto item = std::make_unique<MovePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case StopPlayerMovement::opcode:
            {
                auto item = std::make_unique<StopPlayerMovement>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }
}
