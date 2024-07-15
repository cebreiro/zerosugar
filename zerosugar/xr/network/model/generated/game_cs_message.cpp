#include "game_cs_message.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network::game::cs
{
    void Ping::Deserialize(PacketReader& reader)
    {
        sequence = reader.Read<int64_t>();
        clientTimePoint = reader.Read<int64_t>();
        serverTimePoint = reader.Read<int64_t>();
    }

    void Ping::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(sequence);
        writer.Write<int64_t>(clientTimePoint);
        writer.Write<int64_t>(serverTimePoint);
    }

    void Authenticate::Deserialize(PacketReader& reader)
    {
        authenticationToken = reader.ReadString();
    }

    void Authenticate::Serialize(PacketWriter& writer) const
    {
        writer.Write(authenticationToken);
    }

    void LoadLevelComplete::Deserialize(PacketReader& reader)
    {
        (void)reader;
    }

    void LoadLevelComplete::Serialize(PacketWriter& writer) const
    {
        (void)writer;
    }

    void StartPlayerAttack::Deserialize(PacketReader& reader)
    {
        skillId = reader.Read<int32_t>();
        position = reader.Read<Position>();
        rotation = reader.Read<Rotation>();
    }

    void StartPlayerAttack::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(skillId);
        writer.Write(position);
        writer.Write(rotation);
    }

    void ApplyPlayerAttack::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        targetCount = reader.Read<int32_t>();
        for (int32_t i = 0; i < targetCount; ++i)
        {
            targets.emplace_back(reader.Read<int64_t>());
        }
        skillId = reader.Read<int32_t>();
        position = reader.Read<Position>();
        rotation = reader.Read<Rotation>();
    }

    void ApplyPlayerAttack::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write<int32_t>(targetCount);
        for (const auto& item : targets)
        {
            writer.WriteObject(item);
        }
        writer.Write<int32_t>(skillId);
        writer.Write(position);
        writer.Write(rotation);
    }

    void MovePlayer::Deserialize(PacketReader& reader)
    {
        position = reader.Read<Position>();
        rotation = reader.Read<Rotation>();
    }

    void MovePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write(position);
        writer.Write(rotation);
    }

    void StopPlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        position = reader.Read<Position>();
    }

    void StopPlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(position);
    }

    void SprintPlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
    }

    void SprintPlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
    }

    void RollDodgePlayer::Deserialize(PacketReader& reader)
    {
        id = reader.Read<int64_t>();
        rotation = reader.Read<Rotation>();
    }

    void RollDodgePlayer::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(id);
        writer.Write(rotation);
    }

    void Chat::Deserialize(PacketReader& reader)
    {
        message = reader.ReadString();
    }

    void Chat::Serialize(PacketWriter& writer) const
    {
        writer.Write(message);
    }

    void SwapItem::Deserialize(PacketReader& reader)
    {
        destEquipped = reader.Read<bool>();
        destPosition = reader.Read<int32_t>();
        srcEquipped = reader.Read<bool>();
        srcPosition = reader.Read<int32_t>();
    }

    void SwapItem::Serialize(PacketWriter& writer) const
    {
        writer.Write<bool>(destEquipped);
        writer.Write<int32_t>(destPosition);
        writer.Write<bool>(srcEquipped);
        writer.Write<int32_t>(srcPosition);
    }

    void StartDungeonMatch::Deserialize(PacketReader& reader)
    {
        deugeonId = reader.Read<int32_t>();
    }

    void StartDungeonMatch::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(deugeonId);
    }

    void CancelDungeonMatch::Deserialize(PacketReader& reader)
    {
        (void)reader;
    }

    void CancelDungeonMatch::Serialize(PacketWriter& writer) const
    {
        (void)writer;
    }

    void ApproveDungeonMatch::Deserialize(PacketReader& reader)
    {
        (void)reader;
    }

    void ApproveDungeonMatch::Serialize(PacketWriter& writer) const
    {
        (void)writer;
    }

    void RejectDungeonMatch::Deserialize(PacketReader& reader)
    {
        (void)reader;
    }

    void RejectDungeonMatch::Serialize(PacketWriter& writer) const
    {
        (void)writer;
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case Ping::opcode:
            {
                auto item = std::make_unique<Ping>();
                item->Deserialize(reader);

                return item;
            }
            case Authenticate::opcode:
            {
                auto item = std::make_unique<Authenticate>();
                item->Deserialize(reader);

                return item;
            }
            case LoadLevelComplete::opcode:
            {
                auto item = std::make_unique<LoadLevelComplete>();
                item->Deserialize(reader);

                return item;
            }
            case StartPlayerAttack::opcode:
            {
                auto item = std::make_unique<StartPlayerAttack>();
                item->Deserialize(reader);

                return item;
            }
            case ApplyPlayerAttack::opcode:
            {
                auto item = std::make_unique<ApplyPlayerAttack>();
                item->Deserialize(reader);

                return item;
            }
            case MovePlayer::opcode:
            {
                auto item = std::make_unique<MovePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case StopPlayer::opcode:
            {
                auto item = std::make_unique<StopPlayer>();
                item->Deserialize(reader);

                return item;
            }
            case SprintPlayer::opcode:
            {
                auto item = std::make_unique<SprintPlayer>();
                item->Deserialize(reader);

                return item;
            }
            case RollDodgePlayer::opcode:
            {
                auto item = std::make_unique<RollDodgePlayer>();
                item->Deserialize(reader);

                return item;
            }
            case Chat::opcode:
            {
                auto item = std::make_unique<Chat>();
                item->Deserialize(reader);

                return item;
            }
            case SwapItem::opcode:
            {
                auto item = std::make_unique<SwapItem>();
                item->Deserialize(reader);

                return item;
            }
            case StartDungeonMatch::opcode:
            {
                auto item = std::make_unique<StartDungeonMatch>();
                item->Deserialize(reader);

                return item;
            }
            case CancelDungeonMatch::opcode:
            {
                auto item = std::make_unique<CancelDungeonMatch>();
                item->Deserialize(reader);

                return item;
            }
            case ApproveDungeonMatch::opcode:
            {
                auto item = std::make_unique<ApproveDungeonMatch>();
                item->Deserialize(reader);

                return item;
            }
            case RejectDungeonMatch::opcode:
            {
                auto item = std::make_unique<RejectDungeonMatch>();
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
            case Ping::opcode:
            {
                Ping item;
                item.Deserialize(reader);

                return item;
            }
            case Authenticate::opcode:
            {
                Authenticate item;
                item.Deserialize(reader);

                return item;
            }
            case LoadLevelComplete::opcode:
            {
                LoadLevelComplete item;
                item.Deserialize(reader);

                return item;
            }
            case StartPlayerAttack::opcode:
            {
                StartPlayerAttack item;
                item.Deserialize(reader);

                return item;
            }
            case ApplyPlayerAttack::opcode:
            {
                ApplyPlayerAttack item;
                item.Deserialize(reader);

                return item;
            }
            case MovePlayer::opcode:
            {
                MovePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case StopPlayer::opcode:
            {
                StopPlayer item;
                item.Deserialize(reader);

                return item;
            }
            case SprintPlayer::opcode:
            {
                SprintPlayer item;
                item.Deserialize(reader);

                return item;
            }
            case RollDodgePlayer::opcode:
            {
                RollDodgePlayer item;
                item.Deserialize(reader);

                return item;
            }
            case Chat::opcode:
            {
                Chat item;
                item.Deserialize(reader);

                return item;
            }
            case SwapItem::opcode:
            {
                SwapItem item;
                item.Deserialize(reader);

                return item;
            }
            case StartDungeonMatch::opcode:
            {
                StartDungeonMatch item;
                item.Deserialize(reader);

                return item;
            }
            case CancelDungeonMatch::opcode:
            {
                CancelDungeonMatch item;
                item.Deserialize(reader);

                return item;
            }
            case ApproveDungeonMatch::opcode:
            {
                ApproveDungeonMatch item;
                item.Deserialize(reader);

                return item;
            }
            case RejectDungeonMatch::opcode:
            {
                RejectDungeonMatch item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&
    {
        switch(opcode)
        {
            case Ping::opcode:
            {
                return typeid(Ping);
            }
            case Authenticate::opcode:
            {
                return typeid(Authenticate);
            }
            case LoadLevelComplete::opcode:
            {
                return typeid(LoadLevelComplete);
            }
            case StartPlayerAttack::opcode:
            {
                return typeid(StartPlayerAttack);
            }
            case ApplyPlayerAttack::opcode:
            {
                return typeid(ApplyPlayerAttack);
            }
            case MovePlayer::opcode:
            {
                return typeid(MovePlayer);
            }
            case StopPlayer::opcode:
            {
                return typeid(StopPlayer);
            }
            case SprintPlayer::opcode:
            {
                return typeid(SprintPlayer);
            }
            case RollDodgePlayer::opcode:
            {
                return typeid(RollDodgePlayer);
            }
            case Chat::opcode:
            {
                return typeid(Chat);
            }
            case SwapItem::opcode:
            {
                return typeid(SwapItem);
            }
            case StartDungeonMatch::opcode:
            {
                return typeid(StartDungeonMatch);
            }
            case CancelDungeonMatch::opcode:
            {
                return typeid(CancelDungeonMatch);
            }
            case ApproveDungeonMatch::opcode:
            {
                return typeid(ApproveDungeonMatch);
            }
            case RejectDungeonMatch::opcode:
            {
                return typeid(RejectDungeonMatch);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
