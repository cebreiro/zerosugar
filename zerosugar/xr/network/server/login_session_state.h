#pragma once
#include "zerosugar/shared/state_machine/state_machine.h"
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    ENUM_CLASS(LoginSessionState, int32_t,
        (Connected)
        (Authenticated)
        (TransitionToLobby)
    )

    class LoginServerSessionStateMachine : public StateMachine<LoginSessionState, StateEvent<IPacket, Future<void>>>
    {
    public:
        LoginServerSessionStateMachine(ServiceLocator& serviceLocator, Session& session);
    };

    class ConnectedState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        ConnectedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(const IPacket& inPacket) -> Future<void> override;

    private:
        LoginServerSessionStateMachine& _stateMachine;
        ServiceLocator& _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class AuthenticatedState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        AuthenticatedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(const IPacket& inPacket) -> Future<void> override;

    private:
        LoginServerSessionStateMachine& _stateMachine;
        ServiceLocator& _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class TransitionToLobbyState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        explicit TransitionToLobbyState(Session& session);

        void OnEnter() override;
        auto OnEvent(const IPacket& inPacket) -> Future<void> override;

    private:
        Session& _session;
    };
}
