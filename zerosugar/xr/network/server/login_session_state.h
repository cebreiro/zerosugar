#pragma once
#include "zerosugar/shared/state_machine/state_machine.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/service/model/generated/login_service.h"
#include "zerosugar/xr/service/model/generated/gateway_service.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    ENUM_CLASS(LoginSessionState, int32_t,
        (Connected)
        (Authenticated)
    )

    class LoginServerSessionStateMachine
        : public StateMachine<LoginSessionState, StateEvent<UniquePtrNotNull<IPacket>, Future<void>>>
        , public std::enable_shared_from_this<LoginServerSessionStateMachine>
    {
    public:
        LoginServerSessionStateMachine(ServiceLocator& serviceLocator, Session& session);

        void Start();
        void Shutdown();

        void Receive(Buffer buffer);

        auto GetName() const -> std::string_view;

    private:
        auto Run() -> Future<void>;

    private:
        WeakPtrNotNull<Session> _session;
        std::string _name;
        ServiceLocatorT<ILogService> _serviceLocator;
        std::atomic<bool> _shutdown = false;

        SharedPtrNotNull<Channel<Buffer>> _channel;
    };
}

namespace zerosugar::xr::login
{
    class ConnectedState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        ConnectedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

    private:
        LoginServerSessionStateMachine& _stateMachine;
        ServiceLocatorT<service::ILoginService, service::IGatewayService> _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class AuthenticatedState final : public LoginServerSessionStateMachine::state_type
    {
    public:
        explicit AuthenticatedState(Session& session);

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

    private:
        Session& _session;
    };
}
