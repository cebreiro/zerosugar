#pragma once
#include "zerosugar/shared/network/server/server.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class GameServer final
        : public Server
    {
    public:
        explicit GameServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        ServiceLocator _serviceLocator;

        // std::unordered_map<int64_t, ZoneInstance> _zoneInstances;
    };
}
