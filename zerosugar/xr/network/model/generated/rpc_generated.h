#pragma once
#include <cstdint>
#include <string>
#include <vector>
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
        std::string rpcResult = {};
    };

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>;
}
