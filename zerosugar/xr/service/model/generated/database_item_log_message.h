#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::service
{
    struct EquipItemLog final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t characterId = {};
        int64_t itemId = {};
        int32_t equipPosition = {};
    };

    struct UnequipItemLog final : IPacket
    {
        static constexpr int32_t opcode = 2;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
        int32_t inventorySlot = {};
    };

    struct ShiftItemLog final : IPacket
    {
        static constexpr int32_t opcode = 3;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
        int32_t inventorySlot = {};
    };

    struct DiscardItemLog final : IPacket
    {
        static constexpr int32_t opcode = 4;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
    };

    struct ChangeItemQuantityLog final : IPacket
    {
        static constexpr int32_t opcode = 5;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
        int32_t quantity = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
}
