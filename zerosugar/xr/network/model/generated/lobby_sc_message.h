#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/lobby_message.h"

namespace zerosugar::xr::network::lobby::sc
{
    struct FailAuthenticate final : IPacket
    {
        static constexpr int32_t opcode = 100;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }
    };

    struct ResultCreateCharacter final : IPacket
    {
        static constexpr int32_t opcode = 101;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        bool success = {};
        LobbyCharacter character = {};
    };

    struct SuccessDeleteCharacter final : IPacket
    {
        static constexpr int32_t opcode = 102;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t slot = {};
    };

    struct NotifyCharacterList final : IPacket
    {
        static constexpr int32_t opcode = 103;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t count = {};
        std::vector<LobbyCharacter> character = {};
    };

    struct SuccessSelectCharacter final : IPacket
    {
        static constexpr int32_t opcode = 104;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string ip = {};
        int32_t port = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case FailAuthenticate::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const FailAuthenticate&>);
                visitor.template operator()<FailAuthenticate>(*packet.Cast<FailAuthenticate>());
            }
            break;
            case ResultCreateCharacter::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ResultCreateCharacter&>);
                visitor.template operator()<ResultCreateCharacter>(*packet.Cast<ResultCreateCharacter>());
            }
            break;
            case SuccessDeleteCharacter::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SuccessDeleteCharacter&>);
                visitor.template operator()<SuccessDeleteCharacter>(*packet.Cast<SuccessDeleteCharacter>());
            }
            break;
            case NotifyCharacterList::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const NotifyCharacterList&>);
                visitor.template operator()<NotifyCharacterList>(*packet.Cast<NotifyCharacterList>());
            }
            break;
            case SuccessSelectCharacter::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SuccessSelectCharacter&>);
                visitor.template operator()<SuccessSelectCharacter>(*packet.Cast<SuccessSelectCharacter>());
            }
            break;
        }
    }
}
