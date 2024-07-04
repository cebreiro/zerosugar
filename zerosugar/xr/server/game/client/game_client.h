#pragma once
#include "zerosugar/shared/network/session/id.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    class GameInstance;

    class GameClient final : public IGameController
    {
    public:
        GameClient(WeakPtrNotNull<Session> session, std::string authenticationToken, int64_t accountId, int64_t characterId,
            int64_t worldUserUniqueId, WeakPtrNotNull<GameInstance> gameInstance);
        ~GameClient() override;

        bool IsSubscriberOf(int32_t opcode) const override;

        void Notify(const IPacket& packet) override;

        auto GetControllerId() const -> game_controller_id_type override;
        void SetControllerId(game_controller_id_type id) override;

        void SetSession(WeakPtrNotNull<Session> session);
        void SetGameInstance(WeakPtrNotNull<GameInstance> gameInstance);
        void SetGameEntityId(game_entity_id_type id);

        auto GetSessionId() const -> session::id_type;
        auto GetAuthenticationToken() const -> const std::string&;
        auto GetAccountId() const -> int64_t;
        auto GetCharacterId() const -> int64_t;
        auto GetWorldUserUniqueId() const -> int64_t;
        auto GetGameInstance() -> std::shared_ptr<GameInstance>;
        auto GetGameInstance() const -> std::shared_ptr<GameInstance>;
        auto GetGameEntityId() const -> game_entity_id_type;

    private:
        void Send(Buffer buffer);

    private:
        session::id_type _sessionId;
        WeakPtrNotNull<Session> _session;

        std::string _authenticationToken;
        int64_t _accountId = 0;
        int64_t _characterId = 0;
        int64_t _worldUserUniqueId = 0;
        WeakPtrNotNull<GameInstance> _gameInstance;
        game_controller_id_type _controllerId;
        game_entity_id_type _entityId;
    };
}
