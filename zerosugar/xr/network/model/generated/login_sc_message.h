#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::network::login::sc
{
    struct CreateAccountResult final : IPacket
    {
        static constexpr int32_t opcode = 0;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        bool success = {};
    };

    struct LoginResult final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t errorCode = {};
        std::string authenticationToken = {};
        std::string lobbyIp = {};
        int32_t lobbyPort = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
}
