#pragma once
#include <tbb/concurrent_hash_map.h>

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

    private:
        tbb::concurrent_hash_map<game_instance_id_type, SharedPtrNotNull<GameInstance>> _gameInstances;
    };
}
