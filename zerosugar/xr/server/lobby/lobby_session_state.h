#pragma once
#include "zerosugar/xr/server/lobby/lobby_session_state_machine.h"
#include "zerosugar/xr/service/model/generated/database_service.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

namespace zerosugar::xr::network::lobby::cs
{
    struct CreateCharacter;
    struct DeleteCharacter;
    struct SelectCharacter;
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

    class AuthenticatedState final
        : public LobbyServerSessionStateMachine::state_type
        , public std::enable_shared_from_this<AuthenticatedState>
    {
    public:
        AuthenticatedState(LobbyServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, IUniqueIDGenerator& idGenerator, Session& session);

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;

        bool HasCharacter(int32_t slotId) const;
        auto FindCharacterId(int32_t slotId) const -> std::optional<int64_t>;

        void AddCharacterId(int32_t slotId, int64_t characterId);
        void RemoveCharacterId(int32_t slotId);


    private:
        auto HandlePacket(Session& session, const network::lobby::cs::CreateCharacter& packet) -> Future<void>;
        auto HandlePacket(Session& session, const network::lobby::cs::DeleteCharacter& packet) -> Future<void>;
        auto HandlePacket(Session& session, const network::lobby::cs::SelectCharacter& packet) -> Future<void>;

    private:
        LobbyServerSessionStateMachine& _stateMachine;
        ServiceLocatorT<ILogService, service::IDatabaseService> _serviceLocator;
        IUniqueIDGenerator& _idGenerator;
        WeakPtrNotNull<Session> _session;

        Future<void> _pendingGetCharacterList;
        std::unordered_map<int32_t, int64_t> _slotCharacterIdIndex;
    };

    class TransitionToGameState final : public LobbyServerSessionStateMachine::state_type
    {
    public:
        TransitionToGameState();

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;
    };
}
