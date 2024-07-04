#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
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

        bool success = {};
        std::string authenticationToken = {};
        std::string lobbyIp = {};
        int32_t lobbyPort = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case CreateAccountResult::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const CreateAccountResult&>);
                visitor.template operator()<CreateAccountResult>(*packet.Cast<CreateAccountResult>());
            }
            break;
            case LoginResult::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const LoginResult&>);
                visitor.template operator()<LoginResult>(*packet.Cast<LoginResult>());
            }
            break;
        }
    }
}
