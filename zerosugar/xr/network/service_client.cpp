#include "service_client.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

namespace zerosugar::xr
{
    ServiceClient::ServiceClient(SharedPtrNotNull<execution::AsioStrand> strand)
        : _strand(std::move(strand))
        , _socket(std::make_shared<Socket>(_strand))
    {
        ExpandReceiveBuffer(1024);
        ExpandSendBuffer();
    }

    auto ServiceClient::ConnectAsync(std::string address, uint16_t port) -> Future<void>
    {
        return _socket->ConnectAsync(std::move(address), port).Then(*_strand, [this]()
            {
                Run();
            });
    }

    auto ServiceClient::Run() -> Future<void>
    {
        assert(_socket->IsOpen());
        assert(ExecutionContext::IsEqualTo(*_strand));

        Buffer receivedBuffer;

        while (_socket->IsOpen())
        {
            const std::expected<int64_t, IOError> result = co_await _socket->ReceiveAsync(_receiveBuffer);

            if (result.has_value())
            {
                const int64_t receiveSize = result.value();
                assert(receiveSize <= _receiveBuffer.GetSize());

                receivedBuffer.MergeBack([&]() -> Buffer
                    {
                        Buffer temp;

                        [[maybe_unused]] bool result = _receiveBuffer.SliceFront(temp, receiveSize);
                        assert(result);

                        return temp;
                    }());

                if (receivedBuffer.GetSize() < 4)
                {
                    continue;
                }

                BufferReader reader(receivedBuffer.cbegin(), receivedBuffer.cend());

                const int32_t packetSize = reader.Read<int32_t>();
                if (packetSize < receivedBuffer.GetSize())
                {
                    if (_receiveBuffer.GetSize() < packetSize - 4)
                    {
                        ExpandReceiveBuffer(packetSize);
                    }

                    continue;
                }

                network::service::ResultRemoteProcedureCall rpcResult;
                rpcResult.Deserialize(reader);

                HandleRPCResult(rpcResult);

                assert(reader.GetReadSize() == packetSize);

                Buffer used;
                receivedBuffer.SliceFront(used, packetSize);

                _receiveBuffer.MergeBack(std::move(used));
            }
            else
            {
                // TODO: log
                co_return;
            }
        }
    }

    void ServiceClient::Send(int32_t rpcId, std::string rpcName, std::string param,
        const send_callback_type& callback)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (!_rpcs.try_emplace(rpcId, callback).second)
        {
            assert(false);

            return;
        }

        if (_sendBuffer.GetSize() < 3)
        {
            ExpandSendBuffer();
        }

        Buffer head;
        [[maybe_unused]] bool result = _sendBuffer.SliceFront(head, 4);
        assert(result);

        const int64_t bodySize = [&]()
            {
                BufferWriter writer(_sendBuffer);
                {
                    network::service::RequestRemoteProcedureCall rpcCall;
                    rpcCall.rpcName = std::move(rpcName);
                    rpcCall.rpcId = rpcId;
                    rpcCall.parameter = std::move(param);

                    rpcCall.Serialize(writer);
                }

                return writer.GetWriteSize();
            }();

        Buffer body;
        result = _sendBuffer.SliceFront(body, bodySize);
        assert(result);

        {
            BufferWriter headWriter(head);
            headWriter.Write<int32_t>(static_cast<int32_t>(body.GetSize()) + 4);

            assert(body.GetSize() == bodySize);
        }

        head.MergeBack(std::move(body));

        _socket->SendAsync(std::move(head));
    }

    void ServiceClient::HandleRPCResult(const network::service::ResultRemoteProcedureCall& result)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        auto iter = _rpcs.find(result.rpcId);
        if (iter == _rpcs.end())
        {
            assert(false);

            return;
        }

        const auto callback = std::exchange(iter->second, {});
        _rpcs.erase(iter);

        callback(result.errorCode, result.rpcResult);
    }

    void ServiceClient::ExpandReceiveBuffer(int64_t minSize)
    {
        _receiveBuffer.Add(buffer::Fragment::Create(std::min<int64_t>(minSize, 1024)));
    }

    void ServiceClient::ExpandSendBuffer()
    {
        _sendBuffer.Add(buffer::Fragment::Create(1024));
    }
}
