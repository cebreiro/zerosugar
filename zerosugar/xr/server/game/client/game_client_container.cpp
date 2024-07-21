#include "game_client_container.h"

#include "zerosugar/xr/server/game/client/game_client.h"

namespace zerosugar::xr
{
    bool GameClientContainer::Contains(session::id_type sessionId) const
    {
        std::shared_lock lock(_mutex);

        return _clients.contains(sessionId);
    }

    bool GameClientContainer::Add(session::id_type sessionId, SharedPtrNotNull<GameClient> client)
    {
        assert(client);

        std::unique_lock lock(_mutex);

        if (_clients.try_emplace(sessionId, client).second)
        {
            [[maybe_unused]]
            bool inserted = _userIdIndexer.try_emplace(client->GetWorldUserUniqueId(), client).second;
            assert(inserted);

            inserted = _characterIndexer.try_emplace(client->GetCharacterId(), client).second;
            assert(inserted);

            return true;
        }

        return false;
    }

    bool GameClientContainer::Remove(session::id_type sessionId)
    {
        std::unique_lock lock(_mutex);

        const auto iter = _clients.find(sessionId);
        if (iter == _clients.end())
        {
            return false;
        }

        const int64_t characterId = iter->second->GetCharacterId();

        [[maybe_unused]]
        size_t count = _userIdIndexer.erase(iter->second->GetWorldUserUniqueId());
        assert(count > 0);

        _clients.erase(iter);

        count = _characterIndexer.erase(characterId);
        assert(count > 0);

        return true;
    }

    bool GameClientContainer::Remove(int64_t userId)
    {
        std::unique_lock lock(_mutex);

        const auto iter = _userIdIndexer.find(userId);
        if (iter == _userIdIndexer.end())
        {
            return false;
        }

        const session::id_type id = iter->second->GetSessionId();
        const int64_t characterId = iter->second->GetCharacterId();

        [[maybe_unused]]
        size_t count = _clients.erase(id);
        assert(count > 0);

        _userIdIndexer.erase(iter);

        count = _characterIndexer.erase(characterId);
        assert(count > 0);

        return true;
    }

    auto GameClientContainer::Find(session::id_type sessionId) -> std::shared_ptr<GameClient>
    {
        std::shared_lock lock(_mutex);

        const auto iter = _clients.find(sessionId);

        return iter != _clients.end() ? iter->second : nullptr;
    }

    auto GameClientContainer::FindByUserId(int64_t userId) -> std::shared_ptr<GameClient>
    {
        std::shared_lock lock(_mutex);

        const auto iter = _userIdIndexer.find(userId);

        return iter != _userIdIndexer.end() ? iter->second : nullptr;
    }

    auto GameClientContainer::FindByCharacterId(int64_t characterId) -> std::shared_ptr<GameClient>
    {
        std::shared_lock lock(_mutex);

        const auto iter = _characterIndexer.find(characterId);

        return iter != _characterIndexer.end() ? iter->second : nullptr;
    }

    auto GameClientContainer::GetCount() const -> int64_t
    {
        std::shared_lock lock(_mutex);

        return std::ssize(_clients);
    }
}
