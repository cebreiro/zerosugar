#pragma once
#include "zerosugar/xr/server/login/login_session_state_machine.h"
#include "zerosugar/xr/service/model/generated/login_service.h"
#include "zerosugar/xr/service/model/generated/gateway_service.h"

namespace zerosugar::xr::login
{
    class ConnectedState final : public LoginSessionStateMachine::state_type
    {
    public:
        ConnectedState(LoginSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

    private:
        LoginSessionStateMachine& _stateMachine;
        ServiceLocatorT<service::ILoginService, service::IGatewayService> _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class AuthenticatedState final : public LoginSessionStateMachine::state_type
    {
    public:
        explicit AuthenticatedState(Session& session);

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

    private:
        WeakPtrNotNull<Session> _session;
    };
}
