#include "client.h"

#include "zerosugar/shared/network/buffer/buffer_reader.h"
#include "zerosugar/shared/network/buffer/buffer_writer.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/security/encoder.h"
#include "zerosugar/sl/protocol/security/decoder.h"
#include "zerosugar/sl/server/login/login_packet_handler_container.h"
#include "zerosugar/sl/server/login/handler/login_packet_handler_interface.h"

namespace zerosugar::sl
{
    Client::Client(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand)
        : _locator(std::move(locator))
        , _id(id)
        , _strand(std::move(strand))
    {
        assert(locator.Find<service::ILoginService>());
    }

    Client::~Client()
    {
    }

    void Client::Close()
    {
        _shutdown.store(true);

        if (_loginSession)
        {
            _loginSession->Close();
        }
    }

    void Client::StartLoginPacketProcess(std::shared_ptr<Session> session,
        std::unique_ptr<Decoder> decoder, std::unique_ptr<Encoder> encoder)
    {
        _loginSession = std::move(session);
        _loginPacketDecoder = std::move(decoder);
        _loginPacketEncoder = std::move(encoder);
        _loginBufferChannel = std::make_shared<Channel<Buffer>>();

        Post(*_strand, [self = shared_from_this()]()
            {
                self->RunLoginPacketProcess();
            });
    }

    void Client::StopLoginPacketProcess()
    {
        assert(_loginBufferChannel);
        _loginBufferChannel->Close();
    }

    void Client::ReceiveLoginPacket(Buffer buffer)
    {
        assert(_loginBufferChannel);
        _loginBufferChannel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto Client::ToString() const -> std::string
    {
        std::ostringstream oss;
        oss << "{ id: " << GetId().Unwrap();

        if (_loginSession)
        {
            oss << std::format(", login_session: {}", *_loginSession);
        }

        oss << " }";

        return oss.str();
    }

    auto Client::GetLocator() -> locator_type&
    {
        return _locator;
    }

    auto Client::GetId() const -> const id_type&
    {
        return _id;
    }

    auto Client::RunLoginPacketProcess() -> Future<void>
    {
        assert(_loginSession);
        assert(_loginPacketDecoder);
        assert(_loginPacketEncoder);
        assert(_loginBufferChannel);
        assert(ExecutionContext::GetExecutor() == _strand.get());

        [[maybe_unused]]
        const auto holder = shared_from_this();

        AsyncEnumerable<Buffer> enumerable(_loginBufferChannel);

        try
        {
            while (enumerable.HasNext() && !_shutdown.load())
            {
                Buffer received = co_await enumerable;
                assert(ExecutionContext::GetExecutor() == _strand.get());

                _loginReceiveBuffer.MergeBack(std::move(received));

                while (_loginReceiveBuffer.GetSize() > 2)
                {
                    const int64_t packetSize = [this]()
                        {
                            BufferReader reader(_loginReceiveBuffer.cbegin(), _loginReceiveBuffer.cend());
                            return reader.Read<uint16_t>() + 3;
                        }();

                    if (_loginReceiveBuffer.GetSize() < packetSize)
                    {
                        break;
                    }

                    Buffer packet;
                    [[maybe_unused]] const bool sliced = _loginReceiveBuffer.SliceFront(packet, packetSize);
                    assert(sliced);

                    _loginPacketDecoder->Decode(std::next(packet.begin(), 2), packet.end());
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

                    LoginPacketDeserializeResult result = co_await handler->Handle(*this, packet);
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
                e.what(), *_loginSession));
        }

        co_return;
    }

    void Client::SendLoginPacket(int8_t opcode, Buffer buffer, bool encode)
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        if (encode)
        {
            _loginPacketEncoder->Encode(buffer.begin(), buffer.end());
        }

        Buffer packet = MakePacketHeader(opcode, buffer.GetSize());
        packet.MergeBack(std::move(buffer));

        _loginSession->Send(std::move(packet));
    }

    auto Client::MakePacketHeader(int8_t opcode, int64_t bufferSize) -> Buffer
    {
        assert(ExecutionContext::GetExecutor() == _strand.get());

        if (_loginSendPacketHeaderPool.GetSize() < 3)
        {
            constexpr int64_t expandSize = 64;
            auto ptr = std::make_shared<char[]>(expandSize);

            _loginSendPacketHeaderPool.Add(buffer::Fragment(std::move(ptr), 0, expandSize));
        }

        Buffer header;

        [[maybe_unused]] const bool result = _loginSendPacketHeaderPool.SliceFront(header, 3);
        assert(result);

        BufferWriter writer(header);
        writer.Write<int16_t>(static_cast<int16_t>(bufferSize));
        writer.Write<int8_t>(opcode);

        return header;
    }
}
