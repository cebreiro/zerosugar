#pragma once
#include "zerosugar/sl/server/gateway/gateway_client_id.h"
#include "zerosugar/sl/server/gateway/gateway_client_state.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::sl
{
    class Decoder;
    class Encoder;
    class GatewayServer;

    class GatewayClient final : public std::enable_shared_from_this<GatewayClient>
    {
    public:
        using id_type = gateway_client_id_type;

        using locator_type = ServiceLocatorRef<ILogService, service::ILoginService, service::IWorldService>;

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

        void StartPacketProcess(
            SharedPtrNotNull<GatewayServer> server, SharedPtrNotNull<Session> session,
            UniquePtrNotNull<Decoder> decoder, UniquePtrNotNull<Encoder> encoder);
        void StopPacketProcess();
        void ReceiveLoginPacket(Buffer buffer);

    private:
        auto RunPacketProcess() -> Future<void>;

    private:
        std::atomic<bool> _shutdown = false;
        locator_type _locator;
        id_type _id = id_type::Default();
        SharedPtrNotNull<Strand> _strand;

        GatewayClientState _state = GatewayClientState::Connected;
        SharedPtrNotNull<GatewayServer> _gatewayServer;
        SharedPtrNotNull<Session> _session;
        UniquePtrNotNull<Decoder> _decoder;
        UniquePtrNotNull<Encoder> _encoder;
        SharedPtrNotNull<Channel<Buffer>> _bufferChannel;
    };
}
