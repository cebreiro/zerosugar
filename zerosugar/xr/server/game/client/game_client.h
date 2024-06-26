#pragma once
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/server/game/instance/controller/game_entity_controller_interface.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    class GameInstance;

    class GameClient final : public IGameEntityController
    {
    public:
        GameClient(WeakPtrNotNull<Session> session, std::string authenticationToken, int64_t accountId, int64_t characterId,
            WeakPtrNotNull<GameInstance> gameInstance);
        ~GameClient() override;


        bool IsSubscriberOf(int32_t opcode) const override;

        void Notify(const IPacket& packet) override;

        void SetSession(WeakPtrNotNull<Session> session);

        auto GetAuthenticationToken() const -> const std::string&;
        auto GetAccountId() const -> int64_t;
        auto GetCharacterId() const -> int64_t;
        auto GetGameInstance() -> SharedPtrNotNull<GameInstance>;
        auto GetGameInstance() const -> SharedPtrNotNull<GameInstance>;

    private:
        void Send(Buffer buffer);

    private:
        WeakPtrNotNull<Session> _session;

        std::string _authenticationToken;
        int64_t _accountId = 0;
        int64_t _characterId = 0;
        WeakPtrNotNull<GameInstance> _gameInstance;
    };
}
