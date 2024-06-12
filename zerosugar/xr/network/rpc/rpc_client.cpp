#include "rpc_client.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/rpc/rpc_packet_builder.h"
#include "zerosugar/xr/network/model/generated/rpc_generated.h"

namespace zerosugar::xr
{
    RPCClient::RPCClient(SharedPtrNotNull<execution::AsioExecutor> executor)
        : _executor(std::move(executor))
        , _strand(_executor->MakeStrand())
        , _socket(std::make_shared<Socket>(_strand))
    {
    }

    auto RPCClient::ConnectAsync(std::string address, uint16_t port) -> Future<void>
    {
        return _socket->ConnectAsync(std::move(address), port).Then(*_strand, [this]()
            {
                Run();
            });
    }

    auto RPCClient::RegisterToServer(std::string serviceName, std::string ip, uint16_t port) -> Future<void>
    {
        co_await *_strand;

        assert(_socket->IsOpen());
        assert(!_registers.contains(serviceName));

        network::RequestRegisterRPCClient request;
        request.serviceName = serviceName;
        request.ip = std::move(ip);
        request.port = port;

        const std::expected<int64_t, IOError> ioResult = co_await _socket->SendAsync(RPCPacketBuilder::MakePacket(request));

        if (ioResult.has_value())
        {
            Promise<void> promise;
            Future<void> future = promise.GetFuture();

            _registers[serviceName] = std::move(promise);
            co_await future;

            co_return;
        }
        else
        {
            throw std::runtime_error(std::format("fail to send bytes to rpc server. error: {}", ioResult.error().message));
        }
    }

    auto RPCClient::Run() -> Future<void>
    {
        assert(_socket->IsOpen());
        assert(ExecutionContext::IsEqualTo(*_strand));

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

                if (receivedBuffer.GetSize() < 4)
                {
                    continue;
                }

                BufferReader bufferReader(receivedBuffer.cbegin(), receivedBuffer.cend());
                PacketReader reader(bufferReader);

                const int32_t packetSize = reader.Read<int32_t>();
                if (packetSize < receivedBuffer.GetSize())
                {
                    if (receiveBuffer.GetSize() < packetSize - 4)
                    {
                        receiveBuffer.Add(buffer::Fragment::Create(packetSize));
                    }

                    continue;
                }

                const int32_t opcode = reader.Read<int32_t>();

                switch (opcode)
                {
                case network::ResultRegisterRPCClient::opcode:
                {
                    network::ResultRegisterRPCClient result;
                    result.Deserialize(reader);

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
                    network::RequestRemoteProcedureCall request;
                    request.Deserialize(reader);

                    Post(*_executor, [self = shared_from_this(), request = std::move(request)]() mutable
                        {
                            self->HandleRequestRemoteProcedureCall(std::move(request));
                        });
                }
                break;
                case network::ResultRemoteProcedureCall::opcode:
                {
                    network::ResultRemoteProcedureCall result;
                    result.Deserialize(reader);

                    HandleResultRemoteProcedureCall(result);
                }
                break;
                default:
                    assert(false);
                }

                assert(reader.GetReadSize() == packetSize);

                Buffer used;
                receivedBuffer.SliceFront(used, packetSize);

                receiveBuffer.MergeBack(std::move(used));
            }
            else
            {
                // TODO: log
                co_return;
            }
        }
    }

    void RPCClient::Send(int32_t rpcId, std::string serviceName, std::string rpcName, std::string param, const send_callback_type& callback)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (!_remoteProcedures[serviceName].try_emplace(rpcId, callback).second)
        {
            assert(false);

            return;
        }

        network::RequestRemoteProcedureCall request;
        request.rpcId = rpcId;
        request.serviceName = std::move(serviceName);
        request.rpcName = std::move(rpcName);
        request.parameter = std::move(param);

        Buffer packet = RPCPacketBuilder::MakePacket(request);

        _socket->SendAsync(std::move(packet));
    }

    auto RPCClient::HandleRequestRemoteProcedureCall(network::RequestRemoteProcedureCall request) -> Future<void>
    {
        auto iter = _procedures.find(MakeProcedureKey(request.serviceName, request.rpcName));
        if (iter == _procedures.end())
        {
            network::ResultRemoteProcedureCall resultRPC;
            resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorInvalidRpcName;
            resultRPC.rpcId = request.rpcId;

            _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));

            co_return;
        }

        network::ResultRemoteProcedureCall resultRPC;
        resultRPC.rpcId = request.rpcId;
        resultRPC.serviceName = std::move(request.serviceName);

        try
        {
            std::string result = co_await iter->second(request.parameter);

            resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorNone;
            resultRPC.rpcResult = std::move(result);

            _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));
        }
        catch (...)
        {
            resultRPC.errorCode = network::RemoteProcedureCallErrorCode::RpcErrorInternalError;

            _socket->SendAsync(RPCPacketBuilder::MakePacket(resultRPC));
        }
    }

    void RPCClient::HandleResultRemoteProcedureCall(const network::ResultRemoteProcedureCall& result)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        auto* remoteProcedures = [this, &]()
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

        const auto callback = std::exchange(iter->second, {});
        remoteProcedures->erase(iter);

        assert(callback);
        callback(result.errorCode, result.rpcResult);
    }

    auto RPCClient::MakeProcedureKey(const std::string& service, const std::string& rpc) -> std::string
    {
        return service + rpc;
    }
}
