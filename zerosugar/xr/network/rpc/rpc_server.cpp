#include "rpc_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/rpc/rpc_packet_builder.h"
#include "zerosugar/xr/network/model/generated/rpc_generated.h"
#include "zerosugar/xr/service/orchestrator/orchestrator_service.h"

namespace zerosugar::xr
{
    class RPCServer::ServerImpl : public Server
    {
    public:
        ServerImpl(std::string name, execution::AsioExecutor& executor, RPCServer& rpcServer)
            : Server(std::move(name), executor)
            , _rpcServer(rpcServer)
        {
        }

        void OnAccept(Session& session) override
        {
            _rpcServer.HandleAccept(session);
        }

        void OnReceive(Session& session, Buffer buffer) override
        {
            _rpcServer.HandleReceive(session, std::move(buffer));
        }

        void OnError(Session& session, const boost::system::error_code& error) override
        {
            (void)session;
            (void)error;
        }

    private:
        RPCServer& _rpcServer;
    };

    RPCServer::RPCServer(SharedPtrNotNull<execution::AsioExecutor> executor)
        : _executor(std::move(executor))
        , _server(std::make_shared<ServerImpl>("RPCServer", *_executor, *this))
    {
    }

    void RPCServer::Initialize(ServiceLocator& dependencyLocator)
    {
        IService::Initialize(dependencyLocator);

        _server->Initialize(dependencyLocator);

        _orchestrator = dependencyLocator.Get<OrchestratorService>().shared_from_this();
    }

    void RPCServer::Shutdown()
    {
        IService::Shutdown();
    }

    void RPCServer::Join(std::vector<boost::system::error_code>& errors)
    {
        IService::Join(errors);
    }

    void RPCServer::StartUp(uint16_t port)
    {
        _server->StartUp(port);
    }

    void RPCServer::HandleAccept(Session& session)
    {
        decltype(_receiveBuffers)::accessor accessor;

        if (_receiveBuffers.insert(accessor, session.GetId()))
        {
            accessor->second = std::make_shared<Buffer>();
        }
        else
        {
            assert(false);

            session.Close();
        }
    }

    void RPCServer::HandleReceive(Session& session, Buffer buffer)
    {
        using namespace network;

        SharedPtrNotNull<Buffer> receiveBuffer = nullptr;
        {
            decltype(_receiveBuffers)::accessor accessor;

            if (_receiveBuffers.find(accessor, session.GetId()))
            {
                receiveBuffer = accessor->second;
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

        PacketReader packetReader(receiveBuffer->cbegin(), receiveBuffer->cend());

        const int32_t packetSize = packetReader.Read<int32_t>();
        if (receivedSize < packetSize)
        {
            return;
        }

        const int32_t opcode = packetReader.Read<int32_t>();
        switch (opcode)
        {
        case RequestRegisterRPCClient::opcode:
        {
            RequestRegisterRPCClient request;
            request.Deserialize(packetReader);

            const bool inserted = [&]() -> bool
                {
                    std::shared_ptr<Session> shared = session.shared_from_this();

                    decltype(_sessions)::accessor accessor;
                    if (_sessions.insert(accessor, request.serviceName))
                    {
                        accessor->second = session.shared_from_this();

                        return true;
                    }

                    return false;
                }();

            if (inserted)
            {
                std::lock_guard lock(_serviceSessionIdMutex);
                _sessionServiceLists.emplace(session.GetId(), request.serviceName);
            }

            using enum RemoteProcedureCallErrorCode;

            ResultRegisterRPCClient result;
            result.errorCode = inserted ? RpcErrorNone : RpcErrorDuplicatedServiceName;

            session.Send(RPCPacketBuilder::MakePacket(result));
        }
        break;
        case RequestRemoteProcedureCall::opcode:
        {
            RequestRemoteProcedureCall request;
            request.Deserialize(packetReader);

            _orchestrator->HandleCallRemoteProcedure(request)
                .Then(*_executor,
                    [self = shared_from_this(), sessionId = session.GetId(), request = request]
                    (RemoteProcedureCallErrorCode ec) mutable
                    {
                        do
                        {
                            if (ec != RemoteProcedureCallErrorCode::RpcErrorNone)
                            {
                                break;
                            }

                            std::shared_ptr<Session> session = self->FindSession(request.serviceName);
                            if (!session)
                            {
                                assert(false);

                                ec = RemoteProcedureCallErrorCode::RpcErrorInternalError;

                                break;
                            }

                            session->Send(RPCPacketBuilder::MakePacket(request));

                            return;

                        } while (false);

                        ResultRemoteProcedureCall result;
                        result.errorCode = ec;
                        result.rpcId = request.rpcId;
                        result.serviceName = request.serviceName;

                        self->SendTo(sessionId, RPCPacketBuilder::MakePacket(result));
                    });
        }
        break;
        case ResultRemoteProcedureCall::opcode:
        {
            ResultRemoteProcedureCall result;
            result.Deserialize(packetReader);

            _orchestrator->HandleResultRemoteProcedure(result)
                .Then(*_executor, [self = shared_from_this(), result = result]()
                    {
                        std::shared_ptr<Session> session = self->FindSession(result.serviceName);
                        if (!session)
                        {
                            return;
                        }

                        session->Send(RPCPacketBuilder::MakePacket(result));
                    });
        }
        break;
        default:
            assert(false);

            throw std::runtime_error(std::format("invalid rpc packet opcode: {}", opcode));
        }

        Buffer temp;
        [[maybe_unused]] bool sliced = receiveBuffer->SliceFront(temp, packetSize);
        assert(sliced);
    }

    auto RPCServer::FindSession(const std::string& serviceName) const -> std::shared_ptr<Session>
    {
        decltype(_sessions)::const_accessor accessor;

        if (_sessions.find(accessor, serviceName))
        {
            return accessor->second;
        }

        return {};
    }

    void RPCServer::SendTo(session::id_type id, Buffer buffer)
    {
        if (const std::shared_ptr<Session>& session = _server->FindSession(id))
        {
            session->Send(std::move(buffer));
        }
    }
}
