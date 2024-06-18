#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::coordination::input
{
    struct Authenticate final : IPacket
    {
        static constexpr int32_t opcode = 0;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t serverId = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
}
