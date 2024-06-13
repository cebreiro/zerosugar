#pragma once
#include "zerosugar/sl/server/gateway/gateway_client_id.h"
#include "zerosugar/sl/server/gateway/gateway_client_state.h"
#include "zerosugar/sl/protocol/packet/packet_serializable.h"
#include "zerosugar/sl/protocol/packet/packet_writer.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::sl
{
    class Decoder;
    class Encoder;
    class GatewayPacketHandlerContainer;

    class GatewayClient final : public std::enable_shared_from_this<GatewayClient>
    {
    public:
        using id_type = gateway_client_id_type;

        using locator_type = ServiceLocatorT<ILogService>;

    public:
        GatewayClient() = delete;
        GatewayClient(const GatewayClient& other) = delete;
        GatewayClient(GatewayClient&& other) noexcept = delete;
        GatewayClient& operator=(const GatewayClient& other) = delete;
        GatewayClient& operator=(GatewayClient&& other) noexcept = delete;

    public:
        GatewayClient(locator_type locator, id_type id, SharedPtrNotNull<Strand> strand);
        ~GatewayClient();

        void Close();
        void Close(std::chrono::milliseconds delay);

        void StartReceiveHandler(SharedPtrNotNull<Session> session,
            UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder,
            SharedPtrNotNull<GatewayPacketHandlerContainer> handlers);
        void StopReceiveHandler();
        void Receive(Buffer buffer);

        template <typename T> requires std::derived_from<T, IPacketSerializable>
        void SendPacket(const T& packet);

        auto ToString() const -> std::string;

        auto GetSession() const -> const Session&;
        auto GetId() const -> const id_type&;
        auto GetState() const -> GatewayClientState;
        auto GetAuthToken() const -> const std::string&;
        auto GetAccountId() const -> int64_t;

        void SetState(GatewayClientState state);
        void SetAuthToken(std::string token);
        void SetAccountId(int64_t accountId);

    private:
        auto RunReceiveHandler() -> Future<void>;

        void SendPacket(Buffer buffer);

    private:
        std::atomic<bool> _shutdown = false;
        locator_type _locator;
        id_type _id = id_type::Default();
        SharedPtrNotNull<Strand> _strand;

        GatewayClientState _state = GatewayClientState::Connected;
        SharedPtrNotNull<Session> _session;
        UniquePtrNotNull<Decoder> _decoder;
        UniquePtrNotNull<Encoder> _encoder;
        SharedPtrNotNull<GatewayPacketHandlerContainer> _handlers;
        SharedPtrNotNull<Channel<Buffer>> _bufferChannel;
        Buffer _receiveBuffer;
        Buffer _sendPacketHeaderPool;
        std::queue<Buffer> _sendPackets;

        std::string _authToken;
        int64_t _accountId = 0;
    };

    template <typename T> requires std::derived_from<T, IPacketSerializable>
    void GatewayClient::SendPacket(const T& packet)
    {
        PacketWriter writer;
        writer.Write(packet);

        Post(*_strand, [self = shared_from_this(), buffer = writer.MakePacket()]() mutable
            {
                self->SendPacket(std::move(buffer));
            });
    }
}
