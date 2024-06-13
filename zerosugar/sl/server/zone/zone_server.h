#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/sl/service/generated/world_service_generated_interface.h"

namespace zerosugar::sl
{
    class ServerConfig;

    class ZoneServer final : public Server
    {
        using Server::StartUp;

    public:
        using locator_type = ServiceLocatorT<ILogService, service::IWorldService>;

    public:
        ZoneServer(const ZoneServer& other) = delete;
        ZoneServer(ZoneServer&& other) noexcept = delete;
        ZoneServer& operator=(const ZoneServer& other) = delete;
        ZoneServer& operator=(ZoneServer&& other) noexcept = delete;

        ZoneServer(execution::AsioExecutor& executor, int8_t worldId, int32_t zoneId, uint16_t port);
        ~ZoneServer();

        void Initialize(ServiceLocator& serviceLocator) override;
        void StartUp();
        void Shutdown() override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        locator_type _locator;
        int8_t _worldId = 0;
        int32_t _zoneId = 0;
        uint16_t _port = 0;
    };
}
