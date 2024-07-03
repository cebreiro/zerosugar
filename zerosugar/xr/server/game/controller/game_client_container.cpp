#include "game_client_container.h"

#include "zerosugar/xr/server/game/controller/game_client.h"

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
            const bool inserted = _userIdIndexer.try_emplace(client->GetWorldUserUniqueId(), client).second;
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

        [[maybe_unused]]
        const size_t count = _userIdIndexer.erase(iter->second->GetWorldUserUniqueId());
        assert(count > 0);

        _clients.erase(iter);

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

        [[maybe_unused]]
        const size_t count = _clients.erase(id);
        assert(count > 0);

        _userIdIndexer.erase(iter);

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
}
