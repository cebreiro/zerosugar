#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/lobby_message.h"

namespace zerosugar::xr::network::lobby::cs
{
    struct Authenticate final : IPacket
    {
        static constexpr int32_t opcode = 100;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string authenticationToken = {};
    };

    struct CreateCharacter final : IPacket
    {
        static constexpr int32_t opcode = 102;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        LobbyCharacter character = {};
    };

    struct DeleteCharacter final : IPacket
    {
        static constexpr int32_t opcode = 103;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t slot = {};
    };

    struct SelectCharacter final : IPacket
    {
        static constexpr int32_t opcode = 104;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t slot = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
}
