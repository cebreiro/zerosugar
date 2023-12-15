#pragma once
#include <memory>
#include "zerosugar/shared/network/buffer/buffer.h"
#include "zerosugar/sl/server/login/login_client_id.h"
#include "zerosugar/sl/server/login/login_client_state.h"
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

    class LoginClient final : public std::enable_shared_from_this<LoginClient>
    {
    public:
        using id_type = login_client_id_type;

        using locator_type = ServiceLocatorRef<ILogService, service::ILoginService>;

    public:
        LoginClient() = delete;
        LoginClient(const LoginClient& other) = delete;
        LoginClient(LoginClient&& other) noexcept = delete;
        LoginClient& operator=(const LoginClient& other) = delete;
        LoginClient& operator=(LoginClient&& other) noexcept = delete;

        LoginClient(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand);
        ~LoginClient();

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
        auto GetState() const -> LoginClientState;

        void SetState(LoginClientState state);

    private:
        auto RunLoginPacketProcess() -> Future<void>;

        void SendLoginPacket(int8_t opcode, Buffer buffer, bool encode);
        auto MakePacketHeader(int8_t opcode, int64_t bufferSize) -> Buffer;

    private:
        std::atomic<bool> _shutdown = false;
        locator_type _locator;
        id_type _id = id_type::Default();
        SharedPtrNotNull<Strand> _strand;

        LoginClientState _state = LoginClientState::Connected;
        std::shared_ptr<Session> _session;
        std::unique_ptr<Decoder> _decoder;
        std::unique_ptr<Encoder> _encoder;
        std::shared_ptr<Channel<Buffer>> _bufferChannel;
        Buffer _receiveBuffer;
        Buffer _sendPacketHeaderPool;
        std::queue<Buffer> _sendPackets;
    };

    template <login_packet_concept T>
    void LoginClient::SendPacket(const T& packet, bool encode)
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
    struct formatter<zerosugar::sl::LoginClient> : formatter<string>
    {
        auto format(const zerosugar::sl::LoginClient& client, format_context& ctx) const
        {
            return formatter<string>::format(
                std::format("{}", client.ToString()), ctx);
        }
    };
}
