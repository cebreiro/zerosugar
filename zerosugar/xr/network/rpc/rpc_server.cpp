#include "rpc_server.h"

#include <boost/scope/scope_exit.hpp>
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/rpc/rpc_packet_builder.h"
#include "zerosugar/xr/network/model/generated/rpc_message_json.h"

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
        , _server(std::make_shared<ServerImpl>(std::string(GetName()), *_executor, *this))
    {
    }

    void RPCServer::Initialize(ServiceLocator& serviceLocator)
    {
        IService::Initialize(serviceLocator);

        _server->Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
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

    bool RPCServer::IsOpen() const
    {
        return _server->IsOpen();
    }

    auto RPCServer::GetName() const -> std::string_view
    {
        return "rpc_server";
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

        network::NotifySnowflake notify;
        notify.snowflakeValue = _nextSnowFlakeValue.fetch_add(1);

        session.Send(RPCPacket::ToBuffer(notify));
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

        while (true)
        {
            const int64_t receivedSize = receiveBuffer->GetSize();
            if (receivedSize < 4)
            {
                return;
            }

            PacketReader reader(receiveBuffer->cbegin(), receiveBuffer->cend());

            const int32_t packetSize = reader.Read<int32_t>();
            if (receivedSize < packetSize)
            {
                return;
            }

            std::unique_ptr<IPacket> packet = CreateFrom(reader);

            Buffer temp;
            [[maybe_unused]] bool sliced = receiveBuffer->SliceFront(temp, packetSize);
            assert(sliced);

            switch (packet->GetOpcode())
            {
            case RequestRegisterRPCClient::opcode:
            {
                const RequestRegisterRPCClient& request = *packet->Cast<RequestRegisterRPCClient>();

                const bool inserted = [&]() -> bool
                    {
                        decltype(_sessions)::accessor accessor;
                        if (_sessions.insert(accessor, request.serviceName))
                        {
                            accessor->second = session.shared_from_this();

                            return true;
                        }

                        return false;
                    }();

                assert(inserted);

                if (inserted)
                {
                    std::lock_guard lock(_serviceSessionIdMutex);
                    _sessionServiceLists.emplace(session.GetId(), request.serviceName);
                }

                using enum RemoteProcedureCallErrorCode;

                ResultRegisterRPCClient result;
                result.errorCode = inserted ? RpcErrorNone : RpcErrorDuplicatedServiceName;
                result.serviceName = request.serviceName;

                session.Send(RPCPacket::ToBuffer(result));
            }
            break;
            case RequestRemoteProcedureCall::opcode:
            {
                const RequestRemoteProcedureCall& request = *packet->Cast<RequestRemoteProcedureCall>();

                const std::shared_ptr<Session>& target = FindSession(request.serviceName);
                if (target)
                {
                    {
                        decltype(_pendingRPCs)::accessor accessor;

                        if (_pendingRPCs.insert(accessor, request.rpcId))
                        {
                            accessor->second = session.shared_from_this();
                        }
                        else
                        {
                            nlohmann::json j;
                            to_json(j, request);

                            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                                fmt::format("[{}] rpc id is duplicated. id: {}, request: {}",
                                    GetName(), request.rpcId, j.dump()));
                        }
                    }

                    target->Send(RPCPacket::ToBuffer(request));
                }
                else
                {
                    ZEROSUGAR_LOG_ERROR(_serviceLocator,
                        fmt::format("[{}] fail to find service. service_name: {}",
                            GetName(), request.serviceName));

                    ResultRemoteProcedureCall result;
                    result.errorCode = RemoteProcedureCallErrorCode::RpcErrorInternalError;
                    result.rpcId = request.rpcId;
                    result.serviceName = request.serviceName;

                    session.Send(RPCPacket::ToBuffer(result));
                }
            }
            break;
            case ResultRemoteProcedureCall::opcode:
            {
                const ResultRemoteProcedureCall& result = *packet->Cast<ResultRemoteProcedureCall>();

                const std::shared_ptr<Session>& target = FindSession(result.rpcId);
                if (target)
                {
                    target->Send(RPCPacket::ToBuffer(result));

                    _pendingRPCs.erase(result.rpcId);
                }
                else
                {
                    nlohmann::json j;
                    to_json(j, result);

                    ZEROSUGAR_LOG_ERROR(_serviceLocator,
                        fmt::format("[{}] fail to find service. rpc_id: {}, param: {}",
                            GetName(), result.rpcId, j.dump()));
                }
            }
            break;
            case SendServerStreaming::opcode:
            {
                const SendServerStreaming& serverStreaming = *packet->Cast<SendServerStreaming>();

                const std::shared_ptr<Session>& target = FindSession(serverStreaming.rpcId);
                if (target)
                {
                    target->Send(RPCPacket::ToBuffer(serverStreaming));
                }
                else
                {
                    ZEROSUGAR_LOG_ERROR(_serviceLocator,
                        fmt::format("[{}] fail to find service. service_name: {}", GetName(), serverStreaming.serviceName));
                }
            }
            break;
            case SendClientSteaming::opcode:
            {
                const SendClientSteaming& clientSteaming = *packet->Cast<SendClientSteaming>();

                const std::shared_ptr<Session>& target = FindSession(clientSteaming.serviceName);
                if (target)
                {
                    target->Send(RPCPacket::ToBuffer(clientSteaming));
                }
                else
                {
                    ZEROSUGAR_LOG_ERROR(_serviceLocator,
                        fmt::format("[{}] fail to find service. service_name: {}", GetName(), clientSteaming.serviceName));
                }
            }
            break;
            case AbortClientStreamingRPC::opcode:
            {
                const AbortClientStreamingRPC& abort = *packet->Cast<AbortClientStreamingRPC>();

                const std::shared_ptr<Session>& target = FindSession(abort.serviceName);
                if (target)
                {
                    target->Send(RPCPacket::ToBuffer(abort));
                }
                else
                {
                    ZEROSUGAR_LOG_ERROR(_serviceLocator,
                        fmt::format("[{}] fail to find service. service_name: {}", GetName(), abort.serviceName));
                }
            }
            break;
            default:
                assert(false);
            }
        }
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

    auto RPCServer::FindSession(int64_t rpcId) const -> std::shared_ptr<Session>
    {
        decltype(_pendingRPCs)::const_accessor accessor;

        if (_pendingRPCs.find(accessor, rpcId))
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
