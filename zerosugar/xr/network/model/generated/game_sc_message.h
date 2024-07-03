#pragma once
#include <cstdint>
#include <any>
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
        static constexpr int32_t opcode = 1016;

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
}
