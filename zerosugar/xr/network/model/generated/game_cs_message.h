#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/game_message.h"

namespace zerosugar::xr::network::game::cs
{
    struct Authenticate final : IPacket
    {
        static constexpr int32_t opcode = 1000;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string authenticationToken = {};
    };

    struct LoadLevelComplete final : IPacket
    {
        static constexpr int32_t opcode = 1012;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct StartPlayerAttack final : IPacket
    {
        static constexpr int32_t opcode = 1008;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t skillId = {};
        Position position = {};
        Rotation rotation = {};
    };

    struct ApplyPlayerAttack final : IPacket
    {
        static constexpr int32_t opcode = 1009;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        int32_t targetCount = {};
        std::vector<int64_t> targets = {};
        int32_t skillId = {};
        Position position = {};
        Rotation rotation = {};
    };

    struct MovePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        Position position = {};
        Rotation rotation = {};
    };

    struct StopPlayer final : IPacket
    {
        static constexpr int32_t opcode = 1020;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Position position = {};
    };

    struct SprintPlayer final : IPacket
    {
        static constexpr int32_t opcode = 1021;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
    };

    struct RollDodgePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1023;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Rotation rotation = {};
    };

    struct Chat final : IPacket
    {
        static constexpr int32_t opcode = 1011;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string message = {};
    };

    struct SwapItem final : IPacket
    {
        static constexpr int32_t opcode = 1003;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        bool destEquipped = {};
        int32_t destPosition = {};
        bool srcEquipped = {};
        int32_t srcPosition = {};
    };

    struct StartDungeonMatch final : IPacket
    {
        static constexpr int32_t opcode = 3001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t deugeonId = {};
    };

    struct CancelDungeonMatch final : IPacket
    {
        static constexpr int32_t opcode = 3002;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct ApproveDungeonMatch final : IPacket
    {
        static constexpr int32_t opcode = 3003;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct RejectDungeonMatch final : IPacket
    {
        static constexpr int32_t opcode = 3004;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case Authenticate::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const Authenticate&>);
                visitor.template operator()<Authenticate>(*packet.Cast<Authenticate>());
            }
            break;
            case LoadLevelComplete::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const LoadLevelComplete&>);
                visitor.template operator()<LoadLevelComplete>(*packet.Cast<LoadLevelComplete>());
            }
            break;
            case StartPlayerAttack::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const StartPlayerAttack&>);
                visitor.template operator()<StartPlayerAttack>(*packet.Cast<StartPlayerAttack>());
            }
            break;
            case ApplyPlayerAttack::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ApplyPlayerAttack&>);
                visitor.template operator()<ApplyPlayerAttack>(*packet.Cast<ApplyPlayerAttack>());
            }
            break;
            case MovePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const MovePlayer&>);
                visitor.template operator()<MovePlayer>(*packet.Cast<MovePlayer>());
            }
            break;
            case StopPlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const StopPlayer&>);
                visitor.template operator()<StopPlayer>(*packet.Cast<StopPlayer>());
            }
            break;
            case SprintPlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SprintPlayer&>);
                visitor.template operator()<SprintPlayer>(*packet.Cast<SprintPlayer>());
            }
            break;
            case RollDodgePlayer::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RollDodgePlayer&>);
                visitor.template operator()<RollDodgePlayer>(*packet.Cast<RollDodgePlayer>());
            }
            break;
            case Chat::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const Chat&>);
                visitor.template operator()<Chat>(*packet.Cast<Chat>());
            }
            break;
            case SwapItem::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SwapItem&>);
                visitor.template operator()<SwapItem>(*packet.Cast<SwapItem>());
            }
            break;
            case StartDungeonMatch::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const StartDungeonMatch&>);
                visitor.template operator()<StartDungeonMatch>(*packet.Cast<StartDungeonMatch>());
            }
            break;
            case CancelDungeonMatch::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const CancelDungeonMatch&>);
                visitor.template operator()<CancelDungeonMatch>(*packet.Cast<CancelDungeonMatch>());
            }
            break;
            case ApproveDungeonMatch::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ApproveDungeonMatch&>);
                visitor.template operator()<ApproveDungeonMatch>(*packet.Cast<ApproveDungeonMatch>());
            }
            break;
            case RejectDungeonMatch::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RejectDungeonMatch&>);
                visitor.template operator()<RejectDungeonMatch>(*packet.Cast<RejectDungeonMatch>());
            }
            break;
        }
    }
}
