#include "service_client.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/network/service_packet_builder.h"
#include "zerosugar/xr/network/model/generated/service_to_service_generated.h"

namespace zerosugar::xr
{
    ServiceClient::ServiceClient(SharedPtrNotNull<execution::AsioStrand> strand)
        : _strand(std::move(strand))
        , _socket(std::make_shared<Socket>(_strand))
    {
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

        Buffer receiveBuffer;
        receiveBuffer.Add(buffer::Fragment::Create(1024));

        Buffer receivedBuffer;

        while (_socket->IsOpen())
        {
            const std::expected<int64_t, IOError> result = co_await _socket->ReceiveAsync(receiveBuffer);

            if (result.has_value())
            {
                const int64_t receiveSize = result.value();
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

                BufferReader reader(receivedBuffer.cbegin(), receivedBuffer.cend());

                const int32_t packetSize = reader.Read<int32_t>();
                if (packetSize < receivedBuffer.GetSize())
                {
                    if (receiveBuffer.GetSize() < packetSize - 4)
                    {
                        receiveBuffer.Add(buffer::Fragment::Create(packetSize));
                    }

                    continue;
                }

                network::service::ResultRemoteProcedureCall rpcResult;
                rpcResult.Deserialize(reader);

                HandleRPCResult(rpcResult);

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

    void ServiceClient::Send(int32_t rpcId, std::string rpcName, std::string param,
        const send_callback_type& callback)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        if (!_rpcs.try_emplace(rpcId, callback).second)
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
}
