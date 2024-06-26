#include "game_entity_container.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"

namespace zerosugar::xr
{
    bool GameEntityContainer::Add(SharedPtrNotNull<GameEntity> entity)
    {
        const auto id = entity->GetId();

        return _concurrentFlatMap.insert(std::make_pair(id, std::move(entity)));
    }

    bool GameEntityContainer::Remove(game_entity_id_type id)
    {
        return _concurrentFlatMap.erase(id);
    }

    auto GameEntityContainer::Find(game_entity_id_type id) -> SharedPtrNotNull<GameEntity>
    {
        SharedPtrNotNull<GameEntity> result;

        _concurrentFlatMap.cvisit(id, [&result](const auto& pair) 
            {
                assert(pair.second);

                result = pair.second;
            });

        return result;
    }

    auto GameEntityContainer::Find(game_entity_id_type id) const -> SharedPtrNotNull<GameEntity>
    {
        SharedPtrNotNull<GameEntity> result;

        _concurrentFlatMap.cvisit(id, [&result](const auto& pair)
            {
                assert(pair.second);

                result = pair.second;
            });

        return result;
    }
}
