#include "game_instance_container.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"

namespace zerosugar::xr
{
    GameInstanceContainer::~GameInstanceContainer()
    {
    }

    bool GameInstanceContainer::Add(SharedPtrNotNull<GameInstance> instance)
    {
        decltype(_gameInstances)::accessor accessor;

        if (_gameInstances.insert(accessor, instance->GetId()))
        {
            accessor->second = std::move(instance);

            return true;
        }
        else
        {
            return false;
        }
    }

    bool GameInstanceContainer::Remove(game_instance_id_type id)
    {
        return _gameInstances.erase(id);
    }

    auto GameInstanceContainer::Find(game_instance_id_type id) -> SharedPtrNotNull<GameInstance>
    {
        decltype(_gameInstances)::const_accessor accessor;

        if (_gameInstances.find(accessor, id))
        {
            return accessor->second;
        }

        return nullptr;
    }

    auto GameInstanceContainer::Find(game_instance_id_type id) const -> SharedPtrNotNull<const GameInstance>
    {
        decltype(_gameInstances)::const_accessor accessor;

        if (_gameInstances.find(accessor, id))
        {
            return accessor->second;
        }

        return nullptr;
    }
}
