#pragma once
#include "zerosugar/xr/server/lobby/lobby_session_state_machine.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"
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
    class ConnectedState final : public LobbySessionStateMachine::state_type
    {
    public:
        ConnectedState(LobbySessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session);

        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;
        void OnSessionClose() override;

    private:
        LobbySessionStateMachine& _stateMachine;
        ServiceLocatorT<ILogService, service::ILoginService> _serviceLocator;
        WeakPtrNotNull<Session> _session;
    };

    class AuthenticatedState final
        : public LobbySessionStateMachine::state_type
        , public std::enable_shared_from_this<AuthenticatedState>
    {
    public:
        struct CharacterCache
        {
            int64_t characterId = 0;
            std::string name;
            int32_t zoneId = 0;
        };

    public:
        AuthenticatedState(LobbySessionStateMachine& stateMachine, ServiceLocator& serviceLocator, IUniqueIDGenerator& idGenerator, Session& session);

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;
        void OnSessionClose() override;

        bool HasCharacter(int32_t slotId) const;
        auto FindCharacter(int32_t slotId) const -> const CharacterCache*;

        void AddCharacter(int32_t slotId, const CharacterCache& character);
        void RemoveCharacter(int32_t slotId);

    private:
        auto HandlePacket(Session& session, const network::lobby::cs::CreateCharacter& packet) -> Future<void>;
        auto HandlePacket(Session& session, const network::lobby::cs::DeleteCharacter& packet) -> Future<void>;
        auto HandlePacket(Session& session, const network::lobby::cs::SelectCharacter& packet) -> Future<void>;

    private:
        LobbySessionStateMachine& _stateMachine;
        ServiceLocatorT<ILogService, service::ILoginService, service::ICoordinationService, service::IDatabaseService> _serviceLocator;
        IUniqueIDGenerator& _idGenerator;
        WeakPtrNotNull<Session> _session;

        Future<void> _pendingGetCharacterList;
        std::unordered_map<int32_t, CharacterCache> _characters;
    };

    class TransitionToGameState final : public LobbySessionStateMachine::state_type
    {
    public:
        explicit TransitionToGameState(Session& session);

        void OnEnter() override;
        auto OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void> override;
        void OnSessionClose() override;

    private:
        WeakPtrNotNull<Session> _session;
    };
}
