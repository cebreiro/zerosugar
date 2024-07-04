#pragma once
#include "zerosugar/shared/network/session/id.h"

namespace zerosugar::xr
{
    class GameClient;

    class GameClientContainer
    {
    public:
        bool Contains(session::id_type sessionId) const;

        bool Add(session::id_type sessionId, SharedPtrNotNull<GameClient> client);
        bool Remove(session::id_type sessionId);
        bool Remove(int64_t userId);

        auto Find(session::id_type sessionId) -> std::shared_ptr<GameClient>;
        auto FindByUserId(int64_t userId) -> std::shared_ptr<GameClient>;

    private:
        mutable std::shared_mutex _mutex;

        std::unordered_map<session::id_type, SharedPtrNotNull<GameClient>> _clients;
        std::unordered_map<int64_t, SharedPtrNotNull<GameClient>> _userIdIndexer;
    };
}
