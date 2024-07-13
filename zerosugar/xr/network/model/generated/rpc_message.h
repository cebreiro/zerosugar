#pragma once
#include <cstdint>
#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar::xr::network
{
    enum class RemoteProcedureCallErrorCode : int32_t
    {
        RpcErrorNone = 0,
        RpcErrorInternalError = 1,
        RpcErrorDuplicatedServiceName = 10001,
        RpcErrorInvalidServiceName = 20001,
        RpcErrorInvalidRpcName = 20002,
        RpcErrorInvalidParameter = 20003,
        RpcErrorStreamingClosedGracefully = 300001,
        RpcErrorStreamingClosedByClient = 300002,
        RpcErrorStreamingClosedByServer = 300003,
    };
    auto GetEnumName(RemoteProcedureCallErrorCode e) -> std::string_view;

    struct RequestRegisterRPCClient final : IPacket
    {
        static constexpr int32_t opcode = 1;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        std::string serviceName = {};
        std::string ip = {};
        int32_t port = {};
    };

    struct ResultRegisterRPCClient final : IPacket
    {
        static constexpr int32_t opcode = 2;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        RemoteProcedureCallErrorCode errorCode = {};
        std::string serviceName = {};
    };

    struct RequestRemoteProcedureCall final : IPacket
    {
        static constexpr int32_t opcode = 3;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t rpcId = {};
        std::string serviceName = {};
        std::string rpcName = {};
        std::string parameter = {};
    };

    struct ResultRemoteProcedureCall final : IPacket
    {
        static constexpr int32_t opcode = 4;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        RemoteProcedureCallErrorCode errorCode = {};
        int32_t rpcId = {};
        std::string serviceName = {};
        std::string rpcName = {};
        std::string rpcResult = {};
    };

    struct SendServerStreaming final : IPacket
    {
        static constexpr int32_t opcode = 5;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t rpcId = {};
        std::string serviceName = {};
        std::string rpcResult = {};
    };

    struct SendClientSteaming final : IPacket
    {
        static constexpr int32_t opcode = 6;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t rpcId = {};
        std::string serviceName = {};
        std::string parameter = {};
    };

    struct AbortClientStreamingRPC final : IPacket
    {
        static constexpr int32_t opcode = 7;

        void Deserialize(PacketReader& reader) final;
        void Serialize(PacketWriter& writer) const final;
        auto GetOpcode() const -> int32_t final { return opcode; }

        int32_t rpcId = {};
        std::string serviceName = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
    auto CreateAnyFrom(PacketReader& reader) -> std::any;
    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&;

    template <typename TVisitor>
    auto Visit(const IPacket& packet, const TVisitor& visitor)
    {
        switch(packet.GetOpcode())
        {
            case RequestRegisterRPCClient::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RequestRegisterRPCClient&>);
                visitor.template operator()<RequestRegisterRPCClient>(*packet.Cast<RequestRegisterRPCClient>());
            }
            break;
            case ResultRegisterRPCClient::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ResultRegisterRPCClient&>);
                visitor.template operator()<ResultRegisterRPCClient>(*packet.Cast<ResultRegisterRPCClient>());
            }
            break;
            case RequestRemoteProcedureCall::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const RequestRemoteProcedureCall&>);
                visitor.template operator()<RequestRemoteProcedureCall>(*packet.Cast<RequestRemoteProcedureCall>());
            }
            break;
            case ResultRemoteProcedureCall::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const ResultRemoteProcedureCall&>);
                visitor.template operator()<ResultRemoteProcedureCall>(*packet.Cast<ResultRemoteProcedureCall>());
            }
            break;
            case SendServerStreaming::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SendServerStreaming&>);
                visitor.template operator()<SendServerStreaming>(*packet.Cast<SendServerStreaming>());
            }
            break;
            case SendClientSteaming::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const SendClientSteaming&>);
                visitor.template operator()<SendClientSteaming>(*packet.Cast<SendClientSteaming>());
            }
            break;
            case AbortClientStreamingRPC::opcode:
            {
                static_assert(std::is_invocable_v<TVisitor, const AbortClientStreamingRPC&>);
                visitor.template operator()<AbortClientStreamingRPC>(*packet.Cast<AbortClientStreamingRPC>());
            }
            break;
        }
    }
}
