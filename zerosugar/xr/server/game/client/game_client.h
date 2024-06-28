#pragma once
#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
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
            WeakPtrNotNull<GameInstance> gameInstance);
        ~GameClient() override;

        bool IsSubscriberOf(int32_t opcode) const override;

        void Notify(const IPacket& packet) override;

        auto GetControllerId() const -> int64_t override;
        void SetControllerId(int64_t id) override;

        void SetSession(WeakPtrNotNull<Session> session);
        void SetGameInstance(WeakPtrNotNull<GameInstance> gameInstance);
        void SetGameEntityId(game_entity_id_type id);

        auto GetAuthenticationToken() const -> const std::string&;
        auto GetAccountId() const -> int64_t;
        auto GetCharacterId() const -> int64_t;
        auto GetGameInstance() -> SharedPtrNotNull<GameInstance>;
        auto GetGameInstance() const -> SharedPtrNotNull<GameInstance>;
        auto GetGameEntityId() const -> game_entity_id_type;

    private:
        void Send(Buffer buffer);

    private:
        WeakPtrNotNull<Session> _session;

        std::string _authenticationToken;
        int64_t _accountId = 0;
        int64_t _characterId = 0;
        WeakPtrNotNull<GameInstance> _gameInstance;
        int64_t _controllerId = 0;
        game_entity_id_type _entityId;
    };
}
