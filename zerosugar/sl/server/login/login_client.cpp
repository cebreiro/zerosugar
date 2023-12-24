#include "login_client.h"

#include "zerosugar/shared/network/buffer/buffer_reader.h"
#include "zerosugar/shared/network/buffer/buffer_writer.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/security/decoder.h"
#include "zerosugar/sl/protocol/security/encoder.h"
#include "zerosugar/sl/server/login/login_packet_handler_container.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl
{
    LoginClient::LoginClient(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand)
        : _locator(std::move(locator))
        , _id(id)
        , _strand(std::move(strand))
    {
        assert(locator.ContainsAll());
    }

    LoginClient::~LoginClient()
    {
    }

    void LoginClient::Close()
    {
        _shutdown.store(true);

        if (_session)
        {
            _session->Close();
        }
    }

    void LoginClient::Close(std::chrono::milliseconds delay)
    {
        Delay(delay).Then(*_strand, [weak = weak_from_this()]()
            {
                if (const std::shared_ptr<LoginClient> instance = weak.lock(); instance)
                {
                    instance->Close();
                }
            });
    }

    void LoginClient::StartReceiveHandler(SharedPtrNotNull<Session> session,
        UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder,
        SharedPtrNotNull<LoginPacketHandlerContainer> handlers)
    {
        _session = std::move(session);
        _decoder = std::move(decoder);
        _encoder = std::move(encoder);
        _bufferChannel = std::make_shared<Channel<Buffer>>();
        _handlers = std::move(handlers);

        Post(*_strand, [self = shared_from_this()]()
            {
                self->RunPacketProcess();
            });
    }

    void LoginClient::StopReceiveHandler()
    {
        assert(_bufferChannel);
        _bufferChannel->Close();
    }

    void LoginClient::Receive(Buffer buffer)
    {
        assert(_bufferChannel);
        _bufferChannel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto LoginClient::ToString() const -> std::string
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

    auto LoginClient::GetId() const -> const id_type&
    {
        return _id;
    }

    auto LoginClient::GetState() const -> LoginClientState
    {
        return _state;
    }

    auto LoginClient::GetAuthToken() const -> const std::string&
    {
        return _authToken;
    }

    auto LoginClient::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    auto LoginClient::GetAccount() const -> const std::string&
    {
        return _account;
    }

    void LoginClient::SetState(LoginClientState state)
    {
        _state = state;
    }

    void LoginClient::SetAuthToken(std::string token)
    {
        _authToken = std::move(token);
    }

    void LoginClient::SetAccountId(int64_t accountId)
    {
        _accountId = accountId;
    }

    void LoginClient::SetAccount(std::string account)
    {
        _account = std::move(account);
    }

    auto LoginClient::RunPacketProcess() -> Future<void>
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

                _receiveBuffer.MergeBack(std::move(received));

                while (_receiveBuffer.GetSize() > 2)
                {
                    const int64_t packetSize = [this]()
                        {
                            BufferReader reader(_receiveBuffer.cbegin(), _receiveBuffer.cend());
                            return reader.Read<uint16_t>() + 3;
                        }();

                    if (_receiveBuffer.GetSize() < packetSize)
                    {
                        break;
                    }

                    Buffer packet;
                    [[maybe_unused]] const bool sliced = _receiveBuffer.SliceFront(packet, packetSize);
                    assert(sliced);

                    _decoder->Decode(std::next(packet.begin(), 2), packet.end());
                    BufferReader packetReader(std::next(packet.cbegin(), 2), packet.cend());

                    const int8_t opcode = packetReader.Read<int8_t>();
                    const ILoginPacketHandler* handler = _handlers->Find(opcode);
                    if (!handler)
                    {
                        ZEROSUGAR_LOG_WAN(_locator, std::format("[sl_client] handler not found. close client. client: {}, packet: {}",
                            ToString(), packet.ToString()));

                        Close();
                        break;
                    }

                    Buffer header;
                    (void)packet.SliceFront(header, 3);
                    assert(header.GetSize() == 3);

                    if (!handler->CanHandle(*this))
                    {
                        ZEROSUGAR_LOG_WAN(_locator, std::format("[sl_client] handler deny execution. close client. client: {}, packet: {}, handler: {}",
                            ToString(), packet.ToString(), handler->GetName()));

                        Close();
                        break;
                    }

                    const LoginPacketDeserializeResult result = co_await handler->Handle(*this, packet);
                    switch (result.errorCode)
                    {
                    case LoginPacketHandlerErrorCode::None:
                    {
                        Buffer read;
                        (void)packet.SliceFront(read, result.readSize);
                        assert(read.GetSize() == result.readSize);
                    }
                    break;
                    case LoginPacketHandlerErrorCode::Fail_ShortLength:
                        break;
                    default:
                    {
                        ZEROSUGAR_LOG_ERROR(_locator, std::format("[sl_client] fail to handle packet. client: {}, error: {}, packet: {}",
                            ToString(), zerosugar::sl::ToString(result.errorCode), packet.ToString()));

                        Close();
                        co_return;
                    }
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_WAN(_locator, std::format("[sl_client] login packet process exception. exception: {}, session: {}",
                e.what(), *_session));
        }

        co_return;
    }

    void LoginClient::SendPacket(int8_t opcode, Buffer buffer, bool encode)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        Buffer packet = MakePacketHeader(opcode, buffer.GetSize());

        if (buffer.GetSize() > 0)
        {
            packet.MergeBack(std::move(buffer));
        }

        if (encode)
        {
            _encoder->Encode(std::next(packet.begin(), 2), packet.end());
        }

        _session->Send(std::move(packet));
    }

    auto LoginClient::MakePacketHeader(int8_t opcode, int64_t bufferSize) -> Buffer
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        if (_sendPacketHeaderPool.GetSize() < 3)
        {
            constexpr int64_t expandSize = 64;
            auto ptr = std::make_shared<char[]>(expandSize);

            _sendPacketHeaderPool.Add(buffer::Fragment(std::move(ptr), 0, expandSize));
        }

        Buffer header;

        [[maybe_unused]] const bool result = _sendPacketHeaderPool.SliceFront(header, 3);
        assert(result);

        BufferWriter writer(header);
        writer.Write<int16_t>(static_cast<int16_t>(bufferSize));
        writer.Write<int8_t>(opcode);

        return header;
    }
}
