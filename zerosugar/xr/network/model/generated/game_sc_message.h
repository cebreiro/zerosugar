#pragma once
#include <cstdint>
#include <string>
#include <vector>
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

    struct AddRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1001;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        RemotePlayer player = {};
    };

    struct RemoveRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1002;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
    };

    struct MoveRemotePlayer final : IPacket
    {
        static constexpr int32_t opcode = 1007;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t id = {};
        Position position = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
}
