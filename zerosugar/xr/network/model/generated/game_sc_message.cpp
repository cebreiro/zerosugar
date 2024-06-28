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
    }

    void MoveRemotePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(position);
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
        }
        return {};
    }
}
