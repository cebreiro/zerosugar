#include "service_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/service_packet_builder.h"
#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

namespace zerosugar::xr
{
    ServiceServer::ServiceServer(std::string name, execution::AsioExecutor& executor)
        : Server(std::move(name), executor)
    {
    }

    bool ServiceServer::AddProcedure(std::string name, const std::function<Future<std::string>(const std::string&)>& function)
    {
        return _procedures.try_emplace(std::move(name), function).second;
    }

    void ServiceServer::OnAccept(Session& session)
    {
        decltype(_sessionReceiveBuffers)::accessor accessor;

        if (_sessionReceiveBuffers.insert(accessor, session.GetId()))
        {
            accessor->second = Buffer{};
        }
        else
        {
            assert(false);
        }
    }

    void ServiceServer::OnReceive(Session& session, Buffer buffer)
    {
        Buffer* receiveBuffer = nullptr;

        {
            decltype(_sessionReceiveBuffers)::accessor accessor;

            if (_sessionReceiveBuffers.find(accessor, session.GetId()))
            {
                receiveBuffer = &accessor->second;
            }
        }

        if (!receiveBuffer)
        {
            assert(false);

            return;
        }

        receiveBuffer->MergeBack(std::move(buffer));

        const int64_t receivedSize = receiveBuffer->GetSize();
        if (receivedSize < 4)
        {
            return;
        }

        BufferReader reader(receiveBuffer->cbegin(), receiveBuffer->cend());

        const int64_t packetSize = reader.Read<int32_t>();
        if (receivedSize < packetSize)
        {
            return;
        }

        network::service::RequestRemoteProcedureCall request;
        request.Deserialize(reader);

        auto iter = _procedures.find(request.rpcName);
        if (iter == _procedures.end())
        {
            network::service::ResultRemoteProcedureCall resultRPC;
            resultRPC.errorCode = network::service::RemoteProcedureCallErrorCode::RpcErrorInvalidRpcName;
            resultRPC.rpcId = request.rpcId;

            session.Send(ServicePacketBuilder::MakePacket(resultRPC));

            return;
        }

        iter->second(request.parameter)
            .Then(GetExecutor(),
                [session = session.shared_from_this(), id = request.rpcId](std::string result)
                {
                    network::service::ResultRemoteProcedureCall resultRPC;
                    resultRPC.errorCode = network::service::RemoteProcedureCallErrorCode::RpcErrorNone;
                    resultRPC.rpcId = id;
                    resultRPC.rpcResult = std::move(result);

                    session->Send(ServicePacketBuilder::MakePacket(resultRPC));
                });
    }

    void ServiceServer::OnError(Session& session, const boost::system::error_code& error)
    {
        (void)session;
        (void)error;
    }
}
