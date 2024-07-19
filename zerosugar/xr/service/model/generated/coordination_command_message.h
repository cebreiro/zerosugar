#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::coordination::command
{
    struct LaunchGameInstance final : IPacket
    {
        static constexpr int32_t opcode = 0;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t gameInstanceId = {};
        int32_t zoneId = {};
    };

    struct BroadcastChatting final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string message = {};
    };

    struct NotifyDungeonMatchGroupCreation final : IPacket
    {
        static constexpr int32_t opcode = 2;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t userId = {};
    };

    struct NotifyDungeonMatchGroupApproved final : IPacket
    {
        static constexpr int32_t opcode = 3;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t userId = {};
        std::string ip = {};
        int32_t port = {};
    };

    struct NotifyDungeonMatchGroupRejected final : IPacket
    {
        static constexpr int32_t opcode = 4;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t userId = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case LaunchGameInstance::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const LaunchGameInstance&>);
                visitor.template operator()<LaunchGameInstance>(*packet.Cast<LaunchGameInstance>());
            }
            break;
            case BroadcastChatting::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const BroadcastChatting&>);
                visitor.template operator()<BroadcastChatting>(*packet.Cast<BroadcastChatting>());
            }
            break;
            case NotifyDungeonMatchGroupCreation::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyDungeonMatchGroupCreation&>);
                visitor.template operator()<NotifyDungeonMatchGroupCreation>(*packet.Cast<NotifyDungeonMatchGroupCreation>());
            }
            break;
            case NotifyDungeonMatchGroupApproved::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyDungeonMatchGroupApproved&>);
                visitor.template operator()<NotifyDungeonMatchGroupApproved>(*packet.Cast<NotifyDungeonMatchGroupApproved>());
            }
            break;
            case NotifyDungeonMatchGroupRejected::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyDungeonMatchGroupRejected&>);
                visitor.template operator()<NotifyDungeonMatchGroupRejected>(*packet.Cast<NotifyDungeonMatchGroupRejected>());
            }
            break;
        }
    }
}
