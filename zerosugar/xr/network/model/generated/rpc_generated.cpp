#include "rpc_generated.h"

#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/packet_writer.h"

namespace zerosugar::xr::network
{
    auto GetEnumName(RemoteProcedureCallErrorCode e) -> std::string_view
    {
        switch (e)
        {
            case RemoteProcedureCallErrorCode::RpcErrorNone: return "RpcErrorNone";
            case RemoteProcedureCallErrorCode::RpcErrorInternalError: return "RpcErrorInternalError";
            case RemoteProcedureCallErrorCode::RpcErrorDuplicatedServiceName: return "RpcErrorDuplicatedServiceName";
            case RemoteProcedureCallErrorCode::RpcErrorInvalidServiceName: return "RpcErrorInvalidServiceName";
            case RemoteProcedureCallErrorCode::RpcErrorInvalidRpcName: return "RpcErrorInvalidRpcName";
            case RemoteProcedureCallErrorCode::RpcErrorInvalidParameter: return "RpcErrorInvalidParameter";
        }
        assert(false);
        return "unk";
    }
    void RequestRegisterRPCClient::Deserialize(PacketReader& reader)
    {
        serviceName = reader.ReadString();
        ip = reader.ReadString();
        port = reader.Read<int32_t>();
    }

    void RequestRegisterRPCClient::Serialize(PacketWriter& writer) const
    {
        writer.Write(serviceName);
        writer.Write(ip);
        writer.Write<int32_t>(port);
    }

    void ResultRegisterRPCClient::Deserialize(PacketReader& reader)
    {
        errorCode = reader.Read<RemoteProcedureCallErrorCode>();
        serviceName = reader.ReadString();
    }

    void ResultRegisterRPCClient::Serialize(PacketWriter& writer) const
    {
        writer.WriteObject(errorCode);
        writer.Write(serviceName);
    }

    void RequestRemoteProcedureCall::Deserialize(PacketReader& reader)
    {
        rpcId = reader.Read<int32_t>();
        serviceName = reader.ReadString();
        rpcName = reader.ReadString();
        parameter = reader.ReadString();
    }

    void RequestRemoteProcedureCall::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(rpcId);
        writer.Write(serviceName);
        writer.Write(rpcName);
        writer.Write(parameter);
    }

    void ResultRemoteProcedureCall::Deserialize(PacketReader& reader)
    {
        errorCode = reader.Read<RemoteProcedureCallErrorCode>();
        rpcId = reader.Read<int32_t>();
        serviceName = reader.ReadString();
        rpcResult = reader.ReadString();
    }

    void ResultRemoteProcedureCall::Serialize(PacketWriter& writer) const
    {
        writer.WriteObject(errorCode);
        writer.Write<int32_t>(rpcId);
        writer.Write(serviceName);
        writer.Write(rpcResult);
    }

}
