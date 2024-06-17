#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/xr/server/login/login_session_state.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class LoginServer final
        : public Server
    {
    public:
        explicit LoginServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static std::string_view GetName();

    private:
        ServiceLocator _serviceLocator;

        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<LoginServerSessionStateMachine>> _stateMachines;
    };
}
