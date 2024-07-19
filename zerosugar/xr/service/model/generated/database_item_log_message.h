#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
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

    struct AddItemLog final : IPacket
    {
        static constexpr int32_t opcode = 4;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
        int64_t characterId = {};
        int32_t itemDataId = {};
        int32_t quantity = {};
        int32_t slot = {};
    };

    struct RemoveItemLog final : IPacket
    {
        static constexpr int32_t opcode = 5;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
    };

    struct ChangeItemQuantityLog final : IPacket
    {
        static constexpr int32_t opcode = 6;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t itemId = {};
        int32_t quantity = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case EquipItemLog::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const EquipItemLog&>);
                visitor.template operator()<EquipItemLog>(*packet.Cast<EquipItemLog>());
            }
            break;
            case UnequipItemLog::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const UnequipItemLog&>);
                visitor.template operator()<UnequipItemLog>(*packet.Cast<UnequipItemLog>());
            }
            break;
            case ShiftItemLog::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ShiftItemLog&>);
                visitor.template operator()<ShiftItemLog>(*packet.Cast<ShiftItemLog>());
            }
            break;
            case AddItemLog::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const AddItemLog&>);
                visitor.template operator()<AddItemLog>(*packet.Cast<AddItemLog>());
            }
            break;
            case RemoveItemLog::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RemoveItemLog&>);
                visitor.template operator()<RemoveItemLog>(*packet.Cast<RemoveItemLog>());
            }
            break;
            case ChangeItemQuantityLog::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ChangeItemQuantityLog&>);
                visitor.template operator()<ChangeItemQuantityLog>(*packet.Cast<ChangeItemQuantityLog>());
            }
            break;
        }
    }
}
