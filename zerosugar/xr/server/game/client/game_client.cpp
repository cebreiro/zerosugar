#include "game_client.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet.h"

namespace zerosugar::xr
{
    GameClient::GameClient(WeakPtrNotNull<Session> session, std::string authenticationToken, int64_t accountId, int64_t characterId, WeakPtrNotNull<GameInstance> gameInstance)
        : _session(std::move(session))
        , _authenticationToken(std::move(authenticationToken))
        , _accountId(accountId)
        , _characterId(characterId)
        , _gameInstance(std::move(gameInstance))
    {
    }

    GameClient::~GameClient()
    {
    }

    void GameClient::Notify(const IPacket& packet)
    {
        Send(Packet::ToBuffer(packet));
    }

    auto GameClient::GetControllerId() const -> game_controller_id_type
    {
        return _controllerId;
    }

    void GameClient::SetControllerId(game_controller_id_type id)
    {
        _controllerId = id;
    }

    void GameClient::SetSession(WeakPtrNotNull<Session> session)
    {
        _session = std::move(session);
    }

    void GameClient::SetGameInstance(WeakPtrNotNull<GameInstance> gameInstance)
    {
        _gameInstance = std::move(gameInstance);
    }

    void GameClient::SetGameEntityId(game_entity_id_type id)
    {
        _entityId = id;
    }

    auto GameClient::GetAuthenticationToken() const -> const std::string&
    {
        return _authenticationToken;
    }

    auto GameClient::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    auto GameClient::GetCharacterId() const -> int64_t
    {
        return _characterId;
    }

    void GameClient::Send(Buffer buffer)
    {
        if (const auto session = _session.lock(); session)
        {
            session->Send(std::move(buffer));
        }
    }

    auto GameClient::GetGameInstance() -> SharedPtrNotNull<GameInstance>
    {
        return _gameInstance.lock();
    }

    auto GameClient::GetGameInstance() const -> SharedPtrNotNull<GameInstance>
    {
        return _gameInstance.lock();
    }

    auto GameClient::GetGameEntityId() const -> game_entity_id_type
    {
        return _entityId;
    }

    bool GameClient::IsSubscriberOf(int32_t opcode) const
    {
        (void)opcode;

        return true;
    }
}
