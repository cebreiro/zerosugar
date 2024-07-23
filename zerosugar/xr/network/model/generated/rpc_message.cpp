#include "rpc_message.h"

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
            case RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully: return "RpcErrorStreamingClosedGracefully";
            case RemoteProcedureCallErrorCode::RpcErrorStreamingClosedByClient: return "RpcErrorStreamingClosedByClient";
            case RemoteProcedureCallErrorCode::RpcErrorStreamingClosedByServer: return "RpcErrorStreamingClosedByServer";
        }
        assert(false);
        return "unk";
    }
    void RequestRegisterRPCClient::Deserialize(PacketReader& reader)
    {
        serviceName = reader.ReadString();
    }

    void RequestRegisterRPCClient::Serialize(PacketWriter& writer) const
    {
        writer.Write(serviceName);
    }

    void ResultRegisterRPCClient::Deserialize(PacketReader& reader)
    {
        errorCode = reader.Read<RemoteProcedureCallErrorCode>();
        serviceName = reader.ReadString();
    }

    void ResultRegisterRPCClient::Serialize(PacketWriter& writer) const
    {
        writer.Write(errorCode);
        writer.Write(serviceName);
    }

    void RequestRemoteProcedureCall::Deserialize(PacketReader& reader)
    {
        rpcId = reader.Read<int64_t>();
        serviceName = reader.ReadString();
        rpcName = reader.ReadString();
        parameter = reader.ReadString();
    }

    void RequestRemoteProcedureCall::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(rpcId);
        writer.Write(serviceName);
        writer.Write(rpcName);
        writer.Write(parameter);
    }

    void ResultRemoteProcedureCall::Deserialize(PacketReader& reader)
    {
        errorCode = reader.Read<RemoteProcedureCallErrorCode>();
        rpcId = reader.Read<int64_t>();
        serviceName = reader.ReadString();
        rpcName = reader.ReadString();
        rpcResult = reader.ReadString();
    }

    void ResultRemoteProcedureCall::Serialize(PacketWriter& writer) const
    {
        writer.Write(errorCode);
        writer.Write<int64_t>(rpcId);
        writer.Write(serviceName);
        writer.Write(rpcName);
        writer.Write(rpcResult);
    }

    void SendServerStreaming::Deserialize(PacketReader& reader)
    {
        rpcId = reader.Read<int64_t>();
        serviceName = reader.ReadString();
        rpcResult = reader.ReadString();
    }

    void SendServerStreaming::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(rpcId);
        writer.Write(serviceName);
        writer.Write(rpcResult);
    }

    void SendClientSteaming::Deserialize(PacketReader& reader)
    {
        rpcId = reader.Read<int64_t>();
        serviceName = reader.ReadString();
        parameter = reader.ReadString();
    }

    void SendClientSteaming::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(rpcId);
        writer.Write(serviceName);
        writer.Write(parameter);
    }

    void AbortClientStreamingRPC::Deserialize(PacketReader& reader)
    {
        rpcId = reader.Read<int64_t>();
        serviceName = reader.ReadString();
    }

    void AbortClientStreamingRPC::Serialize(PacketWriter& writer) const
    {
        writer.Write<int64_t>(rpcId);
        writer.Write(serviceName);
    }

    void NotifySnowflake::Deserialize(PacketReader& reader)
    {
        snowflakeValue = reader.Read<int32_t>();
    }

    void NotifySnowflake::Serialize(PacketWriter& writer) const
    {
        writer.Write<int32_t>(snowflakeValue);
    }

    auto CreateFrom(PacketReader& reader) -> std::unique_ptr<IPacket>
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case RequestRegisterRPCClient::opcode:
            {
                auto item = std::make_unique<RequestRegisterRPCClient>();
                item->Deserialize(reader);

                return item;
            }
            case ResultRegisterRPCClient::opcode:
            {
                auto item = std::make_unique<ResultRegisterRPCClient>();
                item->Deserialize(reader);

                return item;
            }
            case RequestRemoteProcedureCall::opcode:
            {
                auto item = std::make_unique<RequestRemoteProcedureCall>();
                item->Deserialize(reader);

                return item;
            }
            case ResultRemoteProcedureCall::opcode:
            {
                auto item = std::make_unique<ResultRemoteProcedureCall>();
                item->Deserialize(reader);

                return item;
            }
            case SendServerStreaming::opcode:
            {
                auto item = std::make_unique<SendServerStreaming>();
                item->Deserialize(reader);

                return item;
            }
            case SendClientSteaming::opcode:
            {
                auto item = std::make_unique<SendClientSteaming>();
                item->Deserialize(reader);

                return item;
            }
            case AbortClientStreamingRPC::opcode:
            {
                auto item = std::make_unique<AbortClientStreamingRPC>();
                item->Deserialize(reader);

                return item;
            }
            case NotifySnowflake::opcode:
            {
                auto item = std::make_unique<NotifySnowflake>();
                item->Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto CreateAnyFrom(PacketReader& reader) -> std::any
    {
        const int16_t opcode = reader.Read<int16_t>();
        switch(opcode)
        {
            case RequestRegisterRPCClient::opcode:
            {
                RequestRegisterRPCClient item;
                item.Deserialize(reader);

                return item;
            }
            case ResultRegisterRPCClient::opcode:
            {
                ResultRegisterRPCClient item;
                item.Deserialize(reader);

                return item;
            }
            case RequestRemoteProcedureCall::opcode:
            {
                RequestRemoteProcedureCall item;
                item.Deserialize(reader);

                return item;
            }
            case ResultRemoteProcedureCall::opcode:
            {
                ResultRemoteProcedureCall item;
                item.Deserialize(reader);

                return item;
            }
            case SendServerStreaming::opcode:
            {
                SendServerStreaming item;
                item.Deserialize(reader);

                return item;
            }
            case SendClientSteaming::opcode:
            {
                SendClientSteaming item;
                item.Deserialize(reader);

                return item;
            }
            case AbortClientStreamingRPC::opcode:
            {
                AbortClientStreamingRPC item;
                item.Deserialize(reader);

                return item;
            }
            case NotifySnowflake::opcode:
            {
                NotifySnowflake item;
                item.Deserialize(reader);

                return item;
            }
        }
        return {};
    }

    auto GetPacketTypeInfo(int32_t opcode) -> const std::type_info&
    {
        switch(opcode)
        {
            case RequestRegisterRPCClient::opcode:
            {
                return typeid(RequestRegisterRPCClient);
            }
            case ResultRegisterRPCClient::opcode:
            {
                return typeid(ResultRegisterRPCClient);
            }
            case RequestRemoteProcedureCall::opcode:
            {
                return typeid(RequestRemoteProcedureCall);
            }
            case ResultRemoteProcedureCall::opcode:
            {
                return typeid(ResultRemoteProcedureCall);
            }
            case SendServerStreaming::opcode:
            {
                return typeid(SendServerStreaming);
            }
            case SendClientSteaming::opcode:
            {
                return typeid(SendClientSteaming);
            }
            case AbortClientStreamingRPC::opcode:
            {
                return typeid(AbortClientStreamingRPC);
            }
            case NotifySnowflake::opcode:
            {
                return typeid(NotifySnowflake);
            }
        }
        assert(false);
        return typeid(nullptr);
    }
}
