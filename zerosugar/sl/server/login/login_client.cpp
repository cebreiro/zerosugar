#include "login_client.h"

#include "zerosugar/shared/network/buffer/buffer_reader.h"
#include "zerosugar/shared/network/buffer/buffer_writer.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/security/encoder.h"
#include "zerosugar/sl/protocol/security/decoder.h"
#include "zerosugar/sl/server/login/login_packet_handler_container.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl
{
    LoginClient::LoginClient(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand)
        : _locator(std::move(locator))
        , _id(id)
        , _strand(std::move(strand))
    {
        assert(locator.Find<service::ILoginService>());
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

    void LoginClient::StartPacketProcess(
        SharedPtrNotNull<LoginServer> loginServer, SharedPtrNotNull<Session> session,
        UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder)
    {
        _loginServer = std::move(loginServer);
        _session = std::move(session);
        _decoder = std::move(decoder);
        _encoder = std::move(encoder);
        _bufferChannel = std::make_shared<Channel<Buffer>>();

        Post(*_strand, [self = shared_from_this()]()
            {
                self->RunPacketProcess();
            });
    }

    void LoginClient::StopPacketProcess()
    {
        assert(_bufferChannel);
        _bufferChannel->Close();
    }

    void LoginClient::ReceiveLoginPacket(Buffer buffer)
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

    auto LoginClient::GetLocator() -> locator_type&
    {
        return _locator;
    }

    auto LoginClient::GetId() const -> const id_type&
    {
        return _id;
    }

    auto LoginClient::GetState() const -> LoginClientState
    {
        return _state;
    }

    void LoginClient::SetState(LoginClientState state)
    {
        _state = state;
    }

    auto LoginClient::RunPacketProcess() -> Future<void>
    {
        assert(_session);
        assert(_decoder);
        assert(_encoder);
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
                    const ILoginPacketHandler* handler = LoginPacketHandlerContainer::GetInstance().Find(opcode);
                    if (!handler)
                    {
                        ZEROSUGAR_LOG_WAN(_locator, std::format("[sl_client] handler not found. close client. client: {}, packet: {}",
                            ToString(), packet.ToString()));

                        Close();
                        break;
                    }

                    Buffer header;
                    (void)packet.SliceFront(header, 3);

                    const LoginPacketDeserializeResult result = co_await handler->Handle(*_loginServer, *this, packet);
                    if (result.errorCode == LoginPacketDeserializeResult::ErrorCode::None)
                    {
                        Buffer _;
                        packet.SliceFront(_, result.readSize);
                    }
                    else
                    {
                        if (result.errorCode == LoginPacketDeserializeResult::ErrorCode::Fail_InvalidFormat)
                        {
                            ZEROSUGAR_LOG_ERROR(_locator, std::format("[sl_client] invalid packet. client: {}, packet: {}",
                                ToString(), packet.ToString()));
                        }
                        else if (result.errorCode == LoginPacketDeserializeResult::ErrorCode::Fail_ShortLength)
                        {
                            ZEROSUGAR_LOG_ERROR(_locator, std::format("[sl_client] packet header length is invalid. client: {}, packet: {}",
                                ToString(), packet.ToString()));
                        }

                        Close();
                        break;
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
        packet.MergeBack(std::move(buffer));

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
