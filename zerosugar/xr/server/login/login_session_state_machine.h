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
