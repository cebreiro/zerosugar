#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/game_message.h"

namespace zerosugar::xr::network::game::sc
{
    struct EnterGame final : IPacket
    {
        static constexpr int32_t opcode = 1000;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t zoneId = {};
        int32_t remotePlayersCount = {};
        std::vector<RemotePlayer> remotePlayers = {};
        int32_t monstersCount = {};
        std::vector<Monster> monsters = {};
        Player localPlayer = {};
    };

    struct NotifyPlayerActivated final : IPacket
    {
        static constexpr int32_t opcode = 1015;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct SpawnRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t playersCount = {};
        std::vector<RemotePlayer> players = {};
    };

    struct AddRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 5001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t playersCount = {};
        std::vector<RemotePlayer> players = {};
    };

    struct RemoveRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1016;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t playersCount = {};
        std::vector<int64_t> players = {};
    };

    struct RemotePlayerAttack final : IPacket
    {
        static constexpr int32_t opcode = 1003;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        int32_t motionId = {};
        Position position = {};
        Rotation rotation = {};
    };

    struct BeAttackedPlayer final : IPacket
    {
        static constexpr int32_t opcode = 5002;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t attackerId = {};
        int64_t attackedId = {};
        int32_t monsterActionIndex = {};
        float attackedHp = {};
    };

    struct SpawnMonster final : IPacket
    {
        static constexpr int32_t opcode = 4001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t monstersCount = {};
        std::vector<Monster> monsters = {};
    };

    struct AddMonster final : IPacket
    {
        static constexpr int32_t opcode = 4002;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t monstersCount = {};
        std::vector<Monster> monsters = {};
    };

    struct RemoveMonster final : IPacket
    {
        static constexpr int32_t opcode = 4003;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t monstersCount = {};
        std::vector<int64_t> monsters = {};
    };

    struct BattleTransitionMonster final : IPacket
    {
        static constexpr int32_t opcode = 4004;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Position position = {};
        Rotation rotation = {};
    };

    struct MoveMonster final : IPacket
    {
        static constexpr int32_t opcode = 4005;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Position position = {};
        Rotation rotation = {};
    };

    struct AttackMonster final : IPacket
    {
        static constexpr int32_t opcode = 4006;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        int32_t actionIndex = {};
        Position position = {};
        Rotation rotation = {};
        Position destPosition = {};
        double destMovementDuration = {};
    };

    struct BeAttackedMonster final : IPacket
    {
        static constexpr int32_t opcode = 5003;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t attackerId = {};
        int64_t attackedId = {};
        float attackedHp = {};
    };

    struct MoveRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1007;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Position position = {};
        Rotation rotation = {};
    };

    struct StopRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1020;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Position position = {};
    };

    struct SprintRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1021;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
    };

    struct RollDodgeRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1004;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Rotation rotation = {};
    };

    struct NotifyChattingMessage final : IPacket
    {
        static constexpr int32_t opcode = 1014;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t type = {};
        std::string message = {};
    };

    struct AddInventory final : IPacket
    {
        static constexpr int32_t opcode = 2001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t itemsCount = {};
        std::vector<PlayerInventoryItem> items = {};
    };

    struct RemoveInventory final : IPacket
    {
        static constexpr int32_t opcode = 2002;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t slotsCount = {};
        std::vector<int32_t> slots = {};
    };

    struct NotifySwapItemResult final : IPacket
    {
        static constexpr int32_t opcode = 2003;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        bool srcEquipment = {};
        bool srcHasItem = {};
        PlayerInventoryItem srcItem = {};
        bool destEquipment = {};
        bool destHasItem = {};
        PlayerInventoryItem destItem = {};
    };

    struct ChangeRemotePlayerEquipItem final : IPacket
    {
        static constexpr int32_t opcode = 1026;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        int32_t equipPosition = {};
        int32_t itemId = {};
    };

    struct NotifyDungeonMatchGroupCreation final : IPacket
    {
        static constexpr int32_t opcode = 1012;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct NotifyDungeonMatchFailure final : IPacket
    {
        static constexpr int32_t opcode = 1013;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct NotifyDungeonMatchGroupApproved final : IPacket
    {
        static constexpr int32_t opcode = 3001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string ip = {};
        int32_t port = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case EnterGame::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const EnterGame&>);
                visitor.template operator()<EnterGame>(*packet.Cast<EnterGame>());
            }
            break;
            case NotifyPlayerActivated::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyPlayerActivated&>);
                visitor.template operator()<NotifyPlayerActivated>(*packet.Cast<NotifyPlayerActivated>());
            }
            break;
            case SpawnRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SpawnRemotePlayer&>);
                visitor.template operator()<SpawnRemotePlayer>(*packet.Cast<SpawnRemotePlayer>());
            }
            break;
            case AddRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const AddRemotePlayer&>);
                visitor.template operator()<AddRemotePlayer>(*packet.Cast<AddRemotePlayer>());
            }
            break;
            case RemoveRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RemoveRemotePlayer&>);
                visitor.template operator()<RemoveRemotePlayer>(*packet.Cast<RemoveRemotePlayer>());
            }
            break;
            case RemotePlayerAttack::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RemotePlayerAttack&>);
                visitor.template operator()<RemotePlayerAttack>(*packet.Cast<RemotePlayerAttack>());
            }
            break;
            case BeAttackedPlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const BeAttackedPlayer&>);
                visitor.template operator()<BeAttackedPlayer>(*packet.Cast<BeAttackedPlayer>());
            }
            break;
            case SpawnMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SpawnMonster&>);
                visitor.template operator()<SpawnMonster>(*packet.Cast<SpawnMonster>());
            }
            break;
            case AddMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const AddMonster&>);
                visitor.template operator()<AddMonster>(*packet.Cast<AddMonster>());
            }
            break;
            case RemoveMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RemoveMonster&>);
                visitor.template operator()<RemoveMonster>(*packet.Cast<RemoveMonster>());
            }
            break;
            case BattleTransitionMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const BattleTransitionMonster&>);
                visitor.template operator()<BattleTransitionMonster>(*packet.Cast<BattleTransitionMonster>());
            }
            break;
            case MoveMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const MoveMonster&>);
                visitor.template operator()<MoveMonster>(*packet.Cast<MoveMonster>());
            }
            break;
            case AttackMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const AttackMonster&>);
                visitor.template operator()<AttackMonster>(*packet.Cast<AttackMonster>());
            }
            break;
            case BeAttackedMonster::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const BeAttackedMonster&>);
                visitor.template operator()<BeAttackedMonster>(*packet.Cast<BeAttackedMonster>());
            }
            break;
            case MoveRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const MoveRemotePlayer&>);
                visitor.template operator()<MoveRemotePlayer>(*packet.Cast<MoveRemotePlayer>());
            }
            break;
            case StopRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const StopRemotePlayer&>);
                visitor.template operator()<StopRemotePlayer>(*packet.Cast<StopRemotePlayer>());
            }
            break;
            case SprintRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SprintRemotePlayer&>);
                visitor.template operator()<SprintRemotePlayer>(*packet.Cast<SprintRemotePlayer>());
            }
            break;
            case RollDodgeRemotePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RollDodgeRemotePlayer&>);
                visitor.template operator()<RollDodgeRemotePlayer>(*packet.Cast<RollDodgeRemotePlayer>());
            }
            break;
            case NotifyChattingMessage::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyChattingMessage&>);
                visitor.template operator()<NotifyChattingMessage>(*packet.Cast<NotifyChattingMessage>());
            }
            break;
            case AddInventory::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const AddInventory&>);
                visitor.template operator()<AddInventory>(*packet.Cast<AddInventory>());
            }
            break;
            case RemoveInventory::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RemoveInventory&>);
                visitor.template operator()<RemoveInventory>(*packet.Cast<RemoveInventory>());
            }
            break;
            case NotifySwapItemResult::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifySwapItemResult&>);
                visitor.template operator()<NotifySwapItemResult>(*packet.Cast<NotifySwapItemResult>());
            }
            break;
            case ChangeRemotePlayerEquipItem::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ChangeRemotePlayerEquipItem&>);
                visitor.template operator()<ChangeRemotePlayerEquipItem>(*packet.Cast<ChangeRemotePlayerEquipItem>());
            }
            break;
            case NotifyDungeonMatchGroupCreation::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyDungeonMatchGroupCreation&>);
                visitor.template operator()<NotifyDungeonMatchGroupCreation>(*packet.Cast<NotifyDungeonMatchGroupCreation>());
            }
            break;
            case NotifyDungeonMatchFailure::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyDungeonMatchFailure&>);
                visitor.template operator()<NotifyDungeonMatchFailure>(*packet.Cast<NotifyDungeonMatchFailure>());
            }
            break;
            case NotifyDungeonMatchGroupApproved::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyDungeonMatchGroupApproved&>);
                visitor.template operator()<NotifyDungeonMatchGroupApproved>(*packet.Cast<NotifyDungeonMatchGroupApproved>());
            }
            break;
        }
    }
}
