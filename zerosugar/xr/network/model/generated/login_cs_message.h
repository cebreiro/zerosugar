#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::network::login::cs
{
    struct CreateAccount final : IPacket
    {
        static constexpr int32_t opcode = 0;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string account = {};
        std::string password = {};
    };

    struct Login final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string account = {};
        std::string password = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case CreateAccount::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const CreateAccount&>);
                visitor.template operator()<CreateAccount>(*packet.Cast<CreateAccount>());
            }
            break;
            case Login::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const Login&>);
                visitor.template operator()<Login>(*packet.Cast<Login>());
            }
            break;
        }
    }
}
