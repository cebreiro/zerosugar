#pragma once
#include "zerosugar/xr/network/server/login_session_state_machine.h"

namespace zerosugar::xr
{
    ENUM_CLASS(LoginSessionState, int32_t,
        (Connected)
        (Authenticated)
        (TransitionToLobby)
    )

    class ConnectedState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        ConnectedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(const IPacket& iPacket) -> Future<void> override;

    private:
        LoginServerSessionStateMachine& _stateMachine;
        ServiceLocator& _serviceLocator;
        Session& _session;
    };

    class AuthenticatedState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        AuthenticatedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(const IPacket& packet) -> Future<void> override;

    private:
        LoginServerSessionStateMachine& _stateMachine;
        ServiceLocator& _serviceLocator;
        Session& _session;
    };

    class TransitionToLobbyState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        explicit TransitionToLobbyState(Session& session);

        void OnEnter() override;
        auto OnEvent(const IPacket& packet) -> Future<void> override;

    private:
        Session& _session;
    };
}
