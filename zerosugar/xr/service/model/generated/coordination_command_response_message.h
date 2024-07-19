#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::coordination::command::response
{
    struct Exception final : IPacket
    {
        static constexpr int32_t opcode = 0;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string message = {};
    };

    struct Authenticate final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t serverId = {};
    };

    struct LaunchGameInstance final : IPacket
    {
        static constexpr int32_t opcode = 2;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int64_t gameInstanceId = {};
        int32_t zoneId = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case Exception::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const Exception&>);
                visitor.template operator()<Exception>(*packet.Cast<Exception>());
            }
            break;
            case Authenticate::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const Authenticate&>);
                visitor.template operator()<Authenticate>(*packet.Cast<Authenticate>());
            }
            break;
            case LaunchGameInstance::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const LaunchGameInstance&>);
                visitor.template operator()<LaunchGameInstance>(*packet.Cast<LaunchGameInstance>());
            }
            break;
        }
    }
}
