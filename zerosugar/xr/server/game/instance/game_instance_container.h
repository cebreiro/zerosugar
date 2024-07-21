#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/game_instance_id.h"

namespace zerosugar::xr
{
    class GameInstance;
}

namespace zerosugar::xr
{
    class GameInstanceContainer
    {
    public:
        ~GameInstanceContainer();

        bool Add(SharedPtrNotNull<GameInstance> instance);
        bool Remove(game_instance_id_type id);

        auto Find(game_instance_id_type id) -> SharedPtrNotNull<GameInstance>;
        auto Find(game_instance_id_type id) const -> SharedPtrNotNull<const GameInstance>;

        void Visit(const std::function<void(GameInstance&)>& function) const;
        void CVisit(const std::function<void(GameInstance&)>& function) const;

        auto GetCount() const -> int64_t;

    private:
        mutable std::shared_mutex _mutex;
        boost::unordered::unordered_flat_map<game_instance_id_type, SharedPtrNotNull<GameInstance>> _gameInstances;
    };
}
