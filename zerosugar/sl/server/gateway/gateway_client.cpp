#include "gateway_client.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/packet/packet_reader.h"
#include "zerosugar/sl/protocol/security/decoder.h"
#include "zerosugar/sl/protocol/security/encoder.h"
#include "zerosugar/sl/server/gateway/gateway_packet_handler_container.h"
#include "zerosugar/sl/server/gateway/handler/gateway_packet_handler_interface.h"

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

    void GatewayClient::Close(std::chrono::milliseconds delay)
    {
        Delay(delay).Then(*_strand, [weak = weak_from_this()]()
            {
                if (const std::shared_ptr<GatewayClient> instance = weak.lock(); instance)
                {
                    instance->Close();
                }
            });
    }

    void GatewayClient::StartReceiveHandler(SharedPtrNotNull<Session> session,
        UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder,
        SharedPtrNotNull<GatewayPacketHandlerContainer> handlers)
    {
        _session = std::move(session);
        _decoder = std::move(decoder);
        _encoder = std::move(encoder);
        _handlers = std::move(handlers);

        _bufferChannel = std::make_shared<Channel<Buffer>>();

        Post(*_strand, [self = shared_from_this()]()
            {
                self->RunReceiveHandler();
            });
    }

    void GatewayClient::StopReceiveHandler()
    {
        assert(_bufferChannel);
        _bufferChannel->Close();
    }

    void GatewayClient::Receive(Buffer buffer)
    {
        assert(_bufferChannel);
        _bufferChannel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto GatewayClient::ToString() const -> std::string
    {
        std::ostringstream oss;
        oss << "{ id: " << GetId().Unwrap();

        if (_session)
        {
            oss << std::format(", login_session: {}", *_session);
        }

        oss << " }";

        return oss.str();
    }

    auto GatewayClient::GetSession() const -> const Session&
    {
        assert(_session);
        return *_session;
    }

    auto GatewayClient::GetId() const -> const id_type&
    {
        return _id;
    }

    auto GatewayClient::GetState() const -> GatewayClientState
    {
        return _state;
    }

    auto GatewayClient::GetAuthToken() const -> const std::string&
    {
        return _authToken;
    }

    auto GatewayClient::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    void GatewayClient::SetState(GatewayClientState state)
    {
        _state = state;
    }

    void GatewayClient::SetAuthToken(std::string token)
    {
        _authToken = std::move(token);
    }

    void GatewayClient::SetAccountId(int64_t accountId)
    {
        _accountId = accountId;
    }

    auto GatewayClient::RunReceiveHandler() -> Future<void>
    {
        assert(_session);
        assert(_decoder);
        assert(_encoder);
        assert(_handlers);
        assert(_bufferChannel);
        assert(ExecutionContext::GetExecutor() == _strand.get());

        [[maybe_unused]]
        const auto holder = shared_from_this();

        AsyncEnumerable<Buffer> enumerable(_bufferChannel);

        try
        {
            while (enumerable.HasNext() && !_shutdown.load())
            {
                Buffer received = co_await enumerable;
                assert(ExecutionContext::GetExecutor() == _strand.get());

                _decoder->Decode(received.begin(), received.end());
                _receiveBuffer.MergeBack(std::move(received));

                while (_receiveBuffer.GetSize() > PacketReader::packet_header_size)
                {
                    const PacketReader::ParseHeaderResult parseResult = PacketReader::ParseHeader(_receiveBuffer);
                    if (parseResult.errorCode == PacketReader::ParseHeaderResult::ErrorCode::ShortLength)
                    {
                        break;
                    }
                    else if (parseResult.errorCode == PacketReader::ParseHeaderResult::ErrorCode::InvalidFormat)
                    {
                        ZEROSUGAR_LOG_WARN(_locator, std::format("[sl_client] invalid packet header detected. client: {}, packet: {}",
                            ToString(), _receiveBuffer.ToString()));

                        Close();
                        break;
                    }

                    assert(parseResult.errorCode == PacketReader::ParseHeaderResult::ErrorCode::None);

                    const int64_t packetSize = parseResult.packetSize;
                    assert(_receiveBuffer.GetSize() >= packetSize);

                    Buffer packet;
                    [[maybe_unused]] const bool sliced = _receiveBuffer.SliceFront(packet, packetSize);
                    assert(sliced);

                    PacketReader reader(packet);

                    const IGatewayPacketHandler* handler = _handlers->Find(reader.Peek<int32_t>());
                    if (!handler)
                    {
                        ZEROSUGAR_LOG_WARN(_locator, std::format("[sl_client] handler not found. close client. client: {}, packet: {}",
                            ToString(), packet.ToString()));

                        Close();
                        break;
                    }

                    if (!handler->CanHandle(*this))
                    {
                        ZEROSUGAR_LOG_WARN(_locator, std::format("[sl_client] handler deny execution. close client. client: {}, packet: {}, handler: {}",
                            ToString(), packet.ToString(), handler->GetName()));

                        Close();
                        break;
                    }

                    co_await handler->Handle(*this, reader);
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_WARN(_locator, std::format("[sl_client] login packet process exception. exception: {}, session: {}",
                e.what(), *_session));
        }

        co_return;
    }

    void GatewayClient::SendPacket(Buffer buffer)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        _encoder->Encode(buffer.begin(), buffer.end());
        _session->Send(std::move(buffer));
    }
}
