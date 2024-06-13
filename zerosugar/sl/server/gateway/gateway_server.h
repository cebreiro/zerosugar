#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/sl/server/gateway/gateway_client_id.h"
#include "zerosugar/sl/service/generated/login_service_generated_interface.h"
#include "zerosugar/sl/service/generated/repository_service_generated_interface.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"

namespace zerosugar::sl
{
    class GatewayPacketHandlerContainer;

    class GatewayServer final : public Server
    {
        using Server::StartUp;

    public:
        static constexpr uint16_t PORT = 2000;

        using locator_type = ServiceLocatorT<ILogService, service::ILoginService,
            service::IRepositoryService, service::IWorldService>;

    public:
        GatewayServer(const GatewayServer& other) = delete;
        GatewayServer(GatewayServer&& other) noexcept = delete;
        GatewayServer& operator=(const GatewayServer& other) = delete;
        GatewayServer& operator=(GatewayServer&& other) noexcept = delete;

        GatewayServer(execution::AsioExecutor& executor, int8_t worldId);
        ~GatewayServer();

        void Initialize(ServiceLocator& serviceLocator) override;
        void StartUp();
        void Shutdown() override;

        auto GetLocator() -> locator_type&;
        auto GetPublicAddress() const -> const std::string&;
        auto GetWorldId() const -> int8_t;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        locator_type _locator;
        int8_t _worldId = 0;
        std::string _publicAddress;
        SharedPtrNotNull<GatewayPacketHandlerContainer> _packetHandlers;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<class GatewayClient>> _clients;
        std::atomic<gateway_client_id_type::value_type> _nextClientId = 0;
    };
}
