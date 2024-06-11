#include "rpc_client.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/service_packet_builder.h"
#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

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
                case network::service::RequestRemoteProcedureCall::opcode:
                {
                    network::service::RequestRemoteProcedureCall request;
                    request.Deserialize(reader);

                    Post(*_executor, [self = shared_from_this(), request = std::move(request)]() mutable
                        {
                            self->HandleRPCRequest(std::move(request));
                        });
                }
                break;
                case network::service::ResultRemoteProcedureCall::opcode:
                {
                    network::service::ResultRemoteProcedureCall result;
                    result.Deserialize(reader);

                    HandleRPCResult(result);
                }
                break;
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

    void RPCClient::Send(int32_t rpcId, std::string rpcName, std::string param, const send_callback_type& callback)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (!_remoteProcedures.try_emplace(rpcId, callback).second)
        {
            assert(false);

            return;
        }

        network::service::RequestRemoteProcedureCall request;
        request.rpcId = rpcId;
        request.rpcName = std::move(rpcName);
        request.parameter = std::move(param);

        Buffer packet = ServicePacketBuilder::MakePacket(request);

        _socket->SendAsync(std::move(packet));
    }

    auto RPCClient::HandleRPCRequest(network::service::RequestRemoteProcedureCall request) -> Future<void>
    {
        auto iter = _procedures.find(request.rpcName);
        if (iter == _procedures.end())
        {
            network::service::ResultRemoteProcedureCall resultRPC;
            resultRPC.errorCode = network::service::RemoteProcedureCallErrorCode::RpcErrorInvalidRpcName;
            resultRPC.rpcId = request.rpcId;

            _socket->SendAsync(ServicePacketBuilder::MakePacket(resultRPC));

            co_return;
        }

        try
        {
            std::string result = co_await iter->second(request.parameter);

            network::service::ResultRemoteProcedureCall resultRPC;
            resultRPC.errorCode = network::service::RemoteProcedureCallErrorCode::RpcErrorNone;
            resultRPC.rpcId = request.rpcId;
            resultRPC.rpcResult = std::move(result);

            _socket->SendAsync(ServicePacketBuilder::MakePacket(resultRPC));
        }
        catch (...)
        {
            network::service::ResultRemoteProcedureCall resultRPC;
            resultRPC.errorCode = network::service::RemoteProcedureCallErrorCode::RpcErrorInternalError;
            resultRPC.rpcId = request.rpcId;

            _socket->SendAsync(ServicePacketBuilder::MakePacket(resultRPC));
        }
    }

    void RPCClient::HandleRPCResult(const network::service::ResultRemoteProcedureCall& result)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        auto iter = _remoteProcedures.find(result.rpcId);
        if (iter == _remoteProcedures.end())
        {
            assert(false);

            return;
        }

        const auto callback = std::exchange(iter->second, {});
        _remoteProcedures.erase(iter);

        assert(callback);
        callback(result.errorCode, result.rpcResult);
    }
}
