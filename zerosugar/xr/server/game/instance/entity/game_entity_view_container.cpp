#include "game_entity_view_container.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity_view.h"

namespace zerosugar::xr
{
    GameEntityViewContainer::~GameEntityViewContainer()
    {
    }

    bool GameEntityViewContainer::Has(game_entity_id_type id) const
    {
        return _entityViews.contains(id);
    }

    bool GameEntityViewContainer::Add(UniquePtrNotNull<GameEntityView> entityView)
    {
        const game_entity_id_type id = entityView->GetId();

        return _entityViews.try_emplace(id, std::move(entityView)).second;
    }

    bool GameEntityViewContainer::Remove(game_entity_id_type id)
    {
        return _entityViews.erase(id);
    }

    auto GameEntityViewContainer::Find(game_entity_id_type id) -> GameEntityView*
    {
        const auto iter = _entityViews.find(id);

        return iter != _entityViews.end() ? iter->second.get() : nullptr;
    }

    auto GameEntityViewContainer::Find(game_entity_id_type id) const -> const GameEntityView*
    {
        const auto iter = _entityViews.find(id);

        return iter != _entityViews.end() ? iter->second.get() : nullptr;
    }
}
