#include "rpc_client.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/rpc/rpc_packet_builder.h"
#include "zerosugar/xr/network/model/generated/rpc_message.h"
#include "zerosugar/xr/network/model/generated/rpc_message_json.h"

namespace zerosugar::xr
{
    RPCClient::RPCClient(SharedPtrNotNull<execution::AsioExecutor> executor)
        : _executor(std::move(executor))
        , _strand(_executor->MakeStrand())
        , _socket(std::make_shared<Socket>(_strand))
    {
    }

    void RPCClient::Initialize(ServiceLocator& serviceLocator)
    {
        _serviceLocator = serviceLocator;
    }

    auto RPCClient::ConnectAsync(std::string address, uint16_t port) -> Future<void>
    {
        return _socket->ConnectAsync(std::move(address), port).Then(*_strand, [self = shared_from_this()]()
            {
                self->Run();
            });
    }

    auto RPCClient::RegisterToServer(std::string serviceName, std::string ip, uint16_t port) -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        co_await *_strand;

        assert(_socket->IsOpen());
        assert(!_registers.contains(serviceName));

        network::RequestRegisterRPCClient request;
        request.serviceName = serviceName;
        request.ip = std::move(ip);
        request.port = port;

        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        _registers[serviceName] = std::move(promise);

        const std::expected<int64_t, IOError> ioResult = co_await _socket->SendAsync(RPCPacketBuilder::MakePacket(request));

        if (ioResult.has_value())
        {
            co_await future;

            co_return;
        }
        else
        {
            throw std::runtime_error(std::format("fail to send bytes to rpc server. error: {}", ioResult.error().message));
        }
    }

    auto RPCClient::GetName() const -> std::string_view
    {
        return "rpc_client";
    }

    auto RPCClient::Run() -> Future<void>
    {
        assert(_socket->IsOpen());
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        auto self = shared_from_this();

        Buffer receiveBuffer;
        receiveBuffer.Add(buffer::Fragment::Create(1024));

        Buffer receivedBuffer;

        while (_socket->IsOpen())
        {
            const std::expected<int64_t, IOError> receiveResult = co_await _socket->ReceiveAsync(receiveBuffer);

            if (receiveResult.has_value())
            {
                const int64_t receiveSize = receiveResult.value();
                assert(receiveSize <= receiveBuffer.GetSize());

                receivedBuffer.MergeBack([&]() -> Buffer
                    {
                        Buffer temp;

                        [[maybe_unused]] bool result = receiveBuffer.SliceFront(temp, receiveSize);
                        assert(result);

                        return temp;
                    }());

                while(true)
                {
                    if (receivedBuffer.GetSize() < 4)
                    {
                        break;
                    }

                    PacketReader reader(receivedBuffer.cbegin(), receivedBuffer.cend());

                    const int32_t packetSize = reader.Read<int32_t>();
                    if (receivedBuffer.GetSize() < packetSize)
                    {
                        if (receiveBuffer.GetSize() < packetSize - 4)
                        {
                            receiveBuffer.Add(buffer::Fragment::Create(packetSize));
                        }

                        break;
                    }

                    std::unique_ptr<IPacket> packet = network::CreateFrom(reader);
                    assert(reader.GetReadSize() == packetSize);

                    Buffer temp;
                    [[maybe_unused]] bool sliced = receivedBuffer.SliceFront(temp, packetSize);
                    assert(sliced);

                    switch (packet->GetOpcode())
                    {
                    case network::ResultRegisterRPCClient::opcode:
                    {
                        const network::ResultRegisterRPCClient& result = *packet->Cast<network::ResultRegisterRPCClient>();

                        const auto iter = _registers.find(result.serviceName);
                        if (iter != _registers.end())
                        {
                            Promise<void> promise = std::exchange(iter->second, {});
                            _registers.erase(iter);

                            try
                            {
                                if (result.errorCode != network::RemoteProcedureCallErrorCode::RpcErrorNone)
                                {
                                    throw std::runtime_error(std::format("fail to register rpc client. error: {}", GetEnumName(result.errorCode)));
                                }

                                promise.Set();
                            }
                            catch (...)
                            {
                                promise.SetException(std::current_exception());
                            }
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    break;
                    case network::RequestRemoteProcedureCall::opcode:
                    {
                        const network::RequestRemoteProcedureCall& request = *packet->Cast<network::RequestRemoteProcedureCall>();

                        HandleRequestRemoteProcedureCall(request);
                    }
                    break;
                    case network::ResultRemoteProcedureCall::opcode:
                    {
                        const network::ResultRemoteProcedureCall& result = *packet->Cast<network::ResultRemoteProcedureCall>();

                        HandleResultRemoteProcedureCall(result);
                    }
                    break;
                    case network::SendServerStreaming::opcode:
                    {
                        const network::SendServerStreaming& serverStreaming = *packet->Cast<network::SendServerStreaming>();

                        HandleServerStreaming(serverStreaming);
                    }
                    break;
                    case network::SendClientSteaming::opcode:
                    {
                        const network::SendClientSteaming& clientStreaming = *packet->Cast<network::SendClientSteaming>();

                        HandleClientStreaming(clientStreaming);
                    }
                    break;
                    case network::AbortClientStreamingRPC::opcode:
                    {
                        const network::AbortClientStreamingRPC& abort = *packet->Cast<network::AbortClientStreamingRPC>();

                        HandleAbortClientStreaming(abort);
                    }
                    break;
                    default:
                        assert(false);
                    }
                }
            }
            else
            {
                ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                    std::format("[rpc_client] receive error. error: {}", receiveResult.error().message));
                
                co_return;
            }
        }
    }

    bool RPCClient::HasCompletionCallback(const std::string& serviceName, int32_t rpcId) const
    {
        const auto iter = _remoteProcedures.find(serviceName);
        if (iter == _remoteProcedures.end())
        {
            return false;
        }

        return iter->second.contains(rpcId);
    }

    void RPCClient::SetCompletionCallback(const std::string& serviceName, int32_t rpcId, const completion_callback_type& callback)
    {
        _remoteProcedures[serviceName][rpcId].second = callback;
    }

    void RPCClient::SetServerStreamingCallback(const std::string& serviceName, int32_t rpcId, const server_streaming_callback& callback)
    {
        _remoteProcedures[serviceName][rpcId].first = callback;
    }

    void RPCClient::Send(int32_t rpcId, std::string serviceName, std::string rpcName, std::string param)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));
        assert(HasCompletionCallback(serviceName, rpcId));

        network::RequestRemoteProcedureCall packet;
        packet.rpcId = rpcId;
        packet.serviceName = std::move(serviceName);
        packet.rpcName = std::move(rpcName);
        packet.parameter = std::move(param);

        Buffer buffer = RPCPacketBuilder::MakePacket(packet);

        _socket->SendAsync(std::move(buffer));
    }

    void RPCClient::SendClientStreaming(int32_t rpcId, std::string serviceName, std::string param)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (!HasCompletionCallback(serviceName, rpcId))
        {
            return;
        }

        network::SendClientSteaming packet;
        packet.rpcId = rpcId;
        packet.serviceName = serviceName;
        packet.parameter = std::move(param);

        Buffer buffer = RPCPacketBuilder::MakePacket(packet);

        _socket->SendAsync(std::move(buffer));
    }

    void RPCClient::SendAbortClientStreaming(int32_t rpcId, const std::string& serviceName)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (!HasCompletionCallback(serviceName, rpcId))
        {
            return;
        }

        network::AbortClientStreamingRPC packet;
        packet.rpcId = rpcId;
        packet.serviceName = serviceName;

        Buffer buffer = RPCPacketBuilder::MakePacket(packet);

        _socket->SendAsync(std::move(buffer));
    }

    auto RPCClient::HandleRequestRemoteProcedureCall(network::RequestRemoteProcedureCall request) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        network::ResultRemoteProcedureCall resultRPC;
        resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorNone;
        resultRPC.rpcId = request.rpcId;
        resultRPC.serviceName = request.serviceName;

        auto iterProcedure = _procedures.find(MakeProcedureKey(request.serviceName, request.rpcName));
        if (iterProcedure == _procedures.end())
        {
            resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorInvalidRpcName;

            _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));

            co_return;
        }

        [[maybe_unused]]
        auto self = shared_from_this();

        if (std::holds_alternative<std::function<Future<std::string>(const std::string&)>>(iterProcedure->second))
        {
            auto& function = std::get<std::function<Future<std::string>(const std::string&)>>(iterProcedure->second);

            try
            {
                resultRPC.rpcResult = co_await function(request.parameter);
            }
            catch (const std::exception& e)
            {
                resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorInternalError;

                ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                    std::format("[rpc_client] rpc throws. id: {}, service: {}, rpc: {}, exception: {}",
                        request.rpcId, request.serviceName, request.rpcName, e.what()));
            }

            _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));
        }
        else if (std::holds_alternative<std::function<AsyncEnumerable<std::string>(const std::string&)>>(iterProcedure->second))
        {
            auto& function = std::get<std::function<AsyncEnumerable<std::string>(const std::string&)>>(iterProcedure->second);

            AsyncEnumerable<std::string> enumerable = function(request.parameter);

            try
            {
                network::SendServerStreaming serverStreaming;
                serverStreaming.rpcId = resultRPC.rpcId;
                serverStreaming.serviceName = resultRPC.serviceName;

                while (enumerable.HasNext())
                {
                    serverStreaming.rpcResult = co_await enumerable;
                    assert(ExecutionContext::IsEqualTo(*self->_strand));

                    _socket->SendAsync(RPCPacketBuilder::MakePacket(serverStreaming));
                }

                resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully;
                resultRPC.rpcResult.clear();
            }
            catch (const AsyncEnumerableClosedException& e)
            {
                (void)e;

                resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully;
                resultRPC.rpcResult.clear();
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                    std::format("[rpc_client] server streaming rpc throws. id: {}, service: {}, rpc: {}, exception: {}",
                        request.rpcId, request.serviceName, request.rpcName, e.what()));

                resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedByServer;
                resultRPC.rpcResult.clear();
            }

            _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));
        }
        else if (std::holds_alternative<std::function<Future<std::string>(SharedPtrNotNull<Channel<std::string>>)>>(iterProcedure->second))
        {
            auto& function = std::get<std::function<Future<std::string>(SharedPtrNotNull<Channel<std::string>>)>>(iterProcedure->second);

            if (!_runningClientStreamingProcedures.contains(request.rpcId))
            {
                auto channel = std::make_shared<Channel<std::string>>();
                _runningClientStreamingProcedures[request.rpcId] = channel;

                channel->Send(request.parameter, channel::ChannelSignal::NotifyOne);

                try
                {
                    resultRPC.rpcResult = co_await function(std::move(channel));
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                        std::format("[rpc_client] client streaming rpc throws. id: {}, service: {}, rpc: {}, exception: {}",
                            request.rpcId, request.serviceName, request.rpcName, e.what()));

                    resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedByServer;
                    resultRPC.rpcResult.clear();
                }

                _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));

                _runningClientStreamingProcedures.erase(request.rpcId);

                co_return;
            }
            else
            {
                assert(false);
            }
        }
        else if (std::holds_alternative<std::function<AsyncEnumerable<std::string>(SharedPtrNotNull<Channel<std::string>>)>>(iterProcedure->second))
        {
            auto& function = std::get<std::function<AsyncEnumerable<std::string>(SharedPtrNotNull<Channel<std::string>>)>>(iterProcedure->second);

            if (!_runningClientStreamingProcedures.contains(request.rpcId))
            {
                auto channel = std::make_shared<Channel<std::string>>();
                _runningClientStreamingProcedures[request.rpcId] = channel;

                channel->Send(request.parameter, channel::ChannelSignal::NotifyOne);
                AsyncEnumerable<std::string> enumerable = function(std::move(channel));

                try
                {
                    network::SendServerStreaming serverStreaming;
                    serverStreaming.rpcId = resultRPC.rpcId;
                    serverStreaming.serviceName = resultRPC.serviceName;

                    while (enumerable.HasNext())
                    {
                        serverStreaming.rpcResult = co_await enumerable;

                        _socket->SendAsync(RPCPacketBuilder::MakePacket(serverStreaming));
                    }

                    resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully;
                    resultRPC.rpcResult.clear();
                }
                catch (const AsyncEnumerableClosedException& e)
                {
                    (void)e;

                    resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedGracefully;
                    resultRPC.rpcResult.clear();
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_ERROR(self->_serviceLocator,
                        std::format("[rpc_client] server-client streaming rpc throws. id: {}, service: {}, rpc: {}, exception: {}",
                            request.rpcId, request.serviceName, request.rpcName, e.what()));

                    resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorStreamingClosedByServer;
                    resultRPC.rpcResult.clear();
                }

                _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));

                _runningClientStreamingProcedures.erase(request.rpcId);
            }
            else
            {
                assert(false);
            }
        }
    }

    void RPCClient::HandleResultRemoteProcedureCall(const network::ResultRemoteProcedureCall& result)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        auto* remoteProcedures = [this, &result]() -> std::unordered_map<int32_t, std::pair<server_streaming_callback, completion_callback_type>>*
            {
                auto iter = _remoteProcedures.find(result.serviceName);
                return iter != _remoteProcedures.end() ? &iter->second : nullptr;
            }();

        if (!remoteProcedures)
        {
            assert(false);

            return;
        }

        auto iter = remoteProcedures->find(result.rpcId);
        if (iter == remoteProcedures->end())
        {
            assert(false);

            return;
        }

        auto callback = std::exchange(iter->second.second, {});
        remoteProcedures->erase(iter);

        assert(callback);
        callback(result.errorCode, result.rpcResult);

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[rpc_client] rpc is removed. rpc_id: {}, service: {}, error: {}",
                result.rpcId, result.serviceName, GetEnumName(result.errorCode)));
    }

    void RPCClient::HandleServerStreaming(const network::SendServerStreaming& serverStreaming)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        auto* remoteProcedures = [this, &serverStreaming]() -> std::unordered_map<int32_t, std::pair<server_streaming_callback, completion_callback_type>>*
            {
                auto iter = _remoteProcedures.find(serverStreaming.serviceName);
                return iter != _remoteProcedures.end() ? &iter->second : nullptr;
            }();

        if (!remoteProcedures)
        {
            assert(false);

            return;
        }

        auto iter = remoteProcedures->find(serverStreaming.rpcId);
        if (iter == remoteProcedures->end())
        {
            assert(false);

            return;
        }

        iter->second.first(serverStreaming.rpcResult);

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[rpc_client] receive server streaming. rpc_id: {}, service: {}",
                serverStreaming.rpcId, serverStreaming.serviceName));
    }

    void RPCClient::HandleClientStreaming(const network::SendClientSteaming& clientStreaming)
    {
        auto iter = _runningClientStreamingProcedures.find(clientStreaming.rpcId);
        if (iter == _runningClientStreamingProcedures.end())
        {
            return;
        }

        iter->second->Send(clientStreaming.parameter, channel::ChannelSignal::NotifyOne);
    }

    void RPCClient::HandleAbortClientStreaming(const network::AbortClientStreamingRPC& abort)
    {
        auto iter = _runningClientStreamingProcedures.find(abort.rpcId);
        if (iter != _runningClientStreamingProcedures.end())
        {
            iter->second->Close();

            _runningClientStreamingProcedures.erase(iter);
        }
    }

    auto RPCClient::MakeProcedureKey(const std::string& service, const std::string& rpc) -> std::string
    {
        assert(!service.empty() && !rpc.empty());

        return service + rpc;
    }
}
