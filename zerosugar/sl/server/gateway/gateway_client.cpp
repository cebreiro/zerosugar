#include "gateway_client.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/security/decoder.h"
#include "zerosugar/sl/protocol/security/encoder.h"

namespace zerosugar::sl
{
    GatewayClient::GatewayClient(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand)
        : _locator(std::move(locator))
        , _id(id)
        , _strand(std::move(strand))
    {
    }

    GatewayClient::~GatewayClient()
    {
    }

    void GatewayClient::Close()
    {
        _shutdown.store(true);

        if (_session)
        {
            _session->Close();
        }
    }

    void GatewayClient::StartPacketProcess(SharedPtrNotNull<GatewayServer> server,
        SharedPtrNotNull<Session> session, UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder)
    {
        _gatewayServer = std::move(server);
        _session = std::move(session);
        _decoder = std::move(decoder);
        _encoder = std::move(encoder);

        _bufferChannel = std::make_shared<Channel<Buffer>>();

        Post(*_strand, [self = shared_from_this()]()
            {
                self->RunPacketProcess();
            });
    }

    void GatewayClient::StopPacketProcess()
    {
        assert(_bufferChannel);
        _bufferChannel->Close();
    }

    void GatewayClient::ReceiveLoginPacket(Buffer buffer)
    {
        assert(_bufferChannel);
        _bufferChannel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto GatewayClient::RunPacketProcess() -> Future<void>
    {
        assert(_session);
        assert(_decoder);
        assert(_encoder);
        assert(_bufferChannel);
        assert(ExecutionContext::GetExecutor() == _strand.get());

        [[maybe_unused]]
        const auto holder = shared_from_this();

        co_return;
    }
}
