#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/xr/network/server/login_session_state.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class LoginServer : public Server
    {
    public:
        explicit LoginServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

        auto SharedFromThis() -> SharedPtrNotNull<LoginServer>;

    private:
        ServiceLocator _serviceLocator;

        tbb::concurrent_hash_map<session::id_type, std::pair<Buffer, SharedPtrNotNull<LoginServerSessionStateMachine>>> _stateMachines;
    };
}
