#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/shared/snowflake/snowflake.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class LobbyServerSessionStateMachine;

    class LobbyServer final
        : public Server
    {
    public:
        explicit LobbyServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;
        void StartUp(uint16_t listenPort) override;

        void SetPublicIP(const std::string& ip);

        static auto GetName() -> std::string_view;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

    private:
        ServiceLocator _serviceLocator;

        std::string _ip;
        std::optional<SharedSnowflake<>> _snowflake;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<LobbyServerSessionStateMachine>> _stateMachines;
    };
}
