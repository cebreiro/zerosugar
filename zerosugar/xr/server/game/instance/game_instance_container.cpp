#include "game_instance_container.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"

namespace zerosugar::xr
{
    GameInstanceContainer::~GameInstanceContainer()
    {
    }

    bool GameInstanceContainer::Add(SharedPtrNotNull<GameInstance> instance)
    {
        const auto id = instance->GetId();

        std::unique_lock lock(_mutex);

        return _gameInstances.try_emplace(id, std::move(instance)).second;
    }

    bool GameInstanceContainer::Remove(game_instance_id_type id)
    {
        std::unique_lock lock(_mutex);

        return _gameInstances.erase(id);
    }

    auto GameInstanceContainer::Find(game_instance_id_type id) -> SharedPtrNotNull<GameInstance>
    {
        std::shared_lock lock(_mutex);

        const auto iter = _gameInstances.find(id);

        return iter != _gameInstances.end() ? iter->second : nullptr;
    }

    auto GameInstanceContainer::Find(game_instance_id_type id) const -> SharedPtrNotNull<const GameInstance>
    {
        std::shared_lock lock(_mutex);

        const auto iter = _gameInstances.find(id);

        return iter != _gameInstances.end() ? iter->second : nullptr;
    }

    void GameInstanceContainer::Visit(const std::function<void(GameInstance&)>& function) const
    {
        assert(function);

        std::shared_lock lock(_mutex);

        for (const SharedPtrNotNull<GameInstance>& gameInstance : _gameInstances | std::views::values)
        {
            function(*gameInstance);
        }
    }

    void GameInstanceContainer::CVisit(const std::function<void(GameInstance&)>& function) const
    {
        assert(function);

        std::shared_lock lock(_mutex);

        for (const SharedPtrNotNull<GameInstance>& gameInstance : _gameInstances | std::views::values)
        {
            function(*gameInstance);
        }
    }

    auto GameInstanceContainer::GetCount() const -> int64_t
    {
        std::shared_lock lock(_mutex);

        return std::ssize(_gameInstances);
    }
}
