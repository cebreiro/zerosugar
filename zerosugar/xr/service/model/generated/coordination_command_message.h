#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
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
}
