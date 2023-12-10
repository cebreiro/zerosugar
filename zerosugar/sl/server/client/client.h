#pragma once
#include <memory>
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/sl/server/client/id.h"
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::sl
{
    class Encoder;
    class Decoder;

    class Client final : public std::enable_shared_from_this<Client>
    {
    public:
        using id_type = client::id_type;

        using locator_type = ServiceLocatorRef<ILogService, service::ILoginService>;

    public:
        Client() = delete;
        Client(const Client& other) = delete;
        Client(Client&& other) noexcept = delete;
        Client& operator=(const Client& other) = delete;
        Client& operator=(Client&& other) noexcept = delete;

        Client(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand);
        ~Client();

        void Close();

        void StartLoginPacketProcess(std::shared_ptr<Session> session,
            std::unique_ptr<Decoder> decoder, std::unique_ptr<Encoder> encoder);
        void StopLoginPacketProcess();
        void ReceiveLoginPacket(Buffer buffer);

        template <login_packet_concept T>
        void SendPacket(const T& packet, bool encode = true);

        auto ToString() const -> std::string;

        auto GetLocator() -> locator_type&;
        auto GetId() const -> const id_type&;

    private:
        auto RunLoginPacketProcess() -> Future<void>;

        void SendLoginPacket(int8_t opcode, Buffer buffer, bool encode);
        auto MakePacketHeader(int8_t opcode, int64_t bufferSize) -> Buffer;

    private:
        std::atomic<bool> _shutdown = false;
        locator_type _locator;
        id_type _id = id_type::Default();
        SharedPtrNotNull<Strand> _strand;

        std::shared_ptr<Session> _loginSession;
        std::unique_ptr<Decoder> _loginPacketDecoder;
        std::unique_ptr<Encoder> _loginPacketEncoder;
        std::shared_ptr<Channel<Buffer>> _loginBufferChannel;
        Buffer _loginReceiveBuffer;
        Buffer _loginSendPacketHeaderPool;
        std::queue<Buffer> _loginSendPackets;
    };

    template <login_packet_concept T>
    void Client::SendPacket(const T& packet, bool encode)
    {
        const int8_t opcode = packet.GetOpcode();
        Buffer buffer = packet.Serialize();

        Post(*_strand, [self = shared_from_this(), opcode, buffer = std::move(buffer), encode]() mutable
            {
                self->SendLoginPacket(opcode, std::move(buffer), encode);
            });
    }
}

namespace std
{
    template <>
    struct formatter<zerosugar::sl::Client> : formatter<string>
    {
        auto format(const zerosugar::sl::Client& client, format_context& ctx) const
        {
            return formatter<string>::format(
                std::format("{}", client.ToString()), ctx);
        }
    };
}
