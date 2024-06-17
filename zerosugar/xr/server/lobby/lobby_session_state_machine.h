#pragma once
#include "zerosugar/shared/state_machine/state_machine.h"
#include "zerosugar/xr/network/packet_interface.h"

namespace zerosugar
{
    class Session;
    class IUniqueIDGenerator;
}

namespace zerosugar::xr
{
    ENUM_CLASS(LobbySessionState, int32_t,
        (Connected)
        (Authenticated)
        (TransitionToGame)
    )

    class LobbyServerSessionStateMachine final
        : public StateMachine<LobbySessionState, StateEvent<UniquePtrNotNull<IPacket>, Future<void>>>
        , public std::enable_shared_from_this<LobbyServerSessionStateMachine>
    {
    public:
        LobbyServerSessionStateMachine(ServiceLocator& serviceLocator, IUniqueIDGenerator& idGenerator, Session& session);

        void Start();
        void Shutdown();

        void Receive(Buffer buffer);

        auto GetName() const -> std::string_view;
        auto GetAccountId() const -> int64_t;

        void SetAccountId(int64_t accountId);

    private:
        auto Run() -> Future<void>;

    private:
        WeakPtrNotNull<Session> _session;
        std::string _name;
        ServiceLocatorT<ILogService> _serviceLocator;
        std::atomic<bool> _shutdown = false;

        SharedPtrNotNull<Channel<Buffer>> _channel;

        int64_t _accountId = 0;
    };
}
