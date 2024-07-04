#include "game_client.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet.h"

namespace zerosugar::xr
{
    GameClient::GameClient(WeakPtrNotNull<Session> session, std::string authenticationToken, int64_t accountId, int64_t characterId,
        int64_t worldUserUniqueId, WeakPtrNotNull<GameInstance> gameInstance)
        : _sessionId(session.lock()->GetId())
        , _session(std::move(session))
        , _authenticationToken(std::move(authenticationToken))
        , _accountId(accountId)
        , _characterId(characterId)
        , _worldUserUniqueId(worldUserUniqueId)
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
        const auto shared = session.lock();
        assert(shared);

        _sessionId = shared->GetId();
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

    auto GameClient::GetSessionId() const -> session::id_type
    {
        return _sessionId;
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

    auto GameClient::GetGameInstance() -> std::shared_ptr<GameInstance>
    {
        return _gameInstance.lock();
    }

    auto GameClient::GetGameInstance() const -> std::shared_ptr<GameInstance>
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

    auto GameClient::GetWorldUserUniqueId() const -> int64_t
    {
        return _worldUserUniqueId;
    }
}
