#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::coordination::command::response
{
    struct Exception final : IPacket
    {
        static constexpr int32_t opcode = 0;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string message = {};
    };

    struct Authenticate final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t serverId = {};
    };

    struct LaunchGameInstance final : IPacket
    {
        static constexpr int32_t opcode = 2;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t gameInstanceId = {};
        int32_t zoneId = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
}
