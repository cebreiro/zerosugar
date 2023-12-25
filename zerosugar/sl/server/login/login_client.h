#pragma once
#include "zerosugar/sl/server/login/login_client_id.h"
#include "zerosugar/sl/server/login/login_client_state.h"
#include "zerosugar/sl/protocol/packet/login/login_packet_concept.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::sl
{
    class Decoder;
    class Encoder;
    class LoginPacketHandlerContainer;

    class LoginClient final : public std::enable_shared_from_this<LoginClient>
    {
    public:
        using id_type = login_client_id_type;

        using locator_type = ServiceLocatorRef<ILogService>;

    public:
        LoginClient() = delete;
        LoginClient(const LoginClient& other) = delete;
        LoginClient(LoginClient&& other) noexcept = delete;
        LoginClient& operator=(const LoginClient& other) = delete;
        LoginClient& operator=(LoginClient&& other) noexcept = delete;

        LoginClient(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand);
        ~LoginClient();

        void Close();
        void Close(std::chrono::milliseconds delay);

        void StartReceiveHandler(SharedPtrNotNull<Session> session,
            UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder,
            SharedPtrNotNull<LoginPacketHandlerContainer> handlers);
        void StopReceiveHandler();
        void Receive(Buffer buffer);

        template <login_packet_concept T>
        void SendPacket(const T& packet, bool encode = true);

        auto ToString() const -> std::string;

        auto GetSession() const -> const Session&;
        auto GetId() const -> const id_type&;
        auto GetState() const -> LoginClientState;
        auto GetAuthToken() const -> const std::string&;
        auto GetAccountId() const -> int64_t;
        auto GetAccount() const -> const std::string&;

        void SetState(LoginClientState state);
        void SetAuthToken(std::string token);
        void SetAccountId(int64_t accountId);
        void SetAccount(std::string account);

    private:
        auto RunReceiveHandler() -> Future<void>;

        void SendPacket(int8_t opcode, Buffer buffer, bool encode);
        auto MakePacketHeader(int8_t opcode, int64_t bufferSize) -> Buffer;

    private:
        std::atomic<bool> _shutdown = false;
        locator_type _locator;
        id_type _id = id_type::Default();
        SharedPtrNotNull<Strand> _strand;

        LoginClientState _state = LoginClientState::Connected;
        SharedPtrNotNull<Session> _session;
        UniquePtrNotNull<Decoder> _decoder;
        UniquePtrNotNull<Encoder> _encoder;
        SharedPtrNotNull<LoginPacketHandlerContainer> _handlers;
        SharedPtrNotNull<Channel<Buffer>> _bufferChannel;
        Buffer _receiveBuffer;
        Buffer _sendPacketHeaderPool;
        std::queue<Buffer> _sendPackets;

        std::string _authToken;
        int64_t _accountId = 0;
        std::string _account;
    };

    template <login_packet_concept T>
    void LoginClient::SendPacket(const T& packet, bool encode)
    {
        const int8_t opcode = packet.GetOpcode();
        Buffer buffer = packet.Serialize();

        Post(*_strand, [self = shared_from_this(), opcode, buffer = std::move(buffer), encode]() mutable
            {
                self->SendPacket(opcode, std::move(buffer), encode);
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
