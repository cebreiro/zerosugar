#pragma once
#include "zerosugar/shared/state_machine/state_machine.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/service/model/generated/database_service.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr::network::lobby::cs
{
    struct CreateCharacter;
    struct DeleteCharacter;
    struct SelectCharacter;
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
        LobbyServerSessionStateMachine(ServiceLocator& serviceLocator, Session& session);

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

namespace zerosugar::xr::lobby
{
    class ConnectedState final : public LobbyServerSessionStateMachine::state_type
    {
    public:
        ConnectedState(LobbyServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

    private:
        LobbyServerSessionStateMachine& _stateMachine;
        ServiceLocatorT<ILogService, service::ILoginService> _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class AuthenticatedState final : public LobbyServerSessionStateMachine::state_type
    {
    public:
        AuthenticatedState(LobbyServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

    private:
        auto HandlePacket(Session& session, const network::lobby::cs::CreateCharacter& packet) -> Future<void>;
        auto HandlePacket(Session& session, const network::lobby::cs::DeleteCharacter& packet) -> Future<void>;
        auto HandlePacket(Session& session, const network::lobby::cs::SelectCharacter& packet) -> Future<void>;

    private:
        LobbyServerSessionStateMachine& _stateMachine;
        ServiceLocatorT<ILogService, service::IDatabaseService> _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class TransitionToGameState final : public LobbyServerSessionStateMachine::state_type
    {
    public:
        TransitionToGameState();

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;
    };
}
