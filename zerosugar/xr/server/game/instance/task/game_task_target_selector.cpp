#include "game_task_target_selector.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_view_container.h"

namespace zerosugar::xr::game_task
{
    MainTargetSelector::MainTargetSelector(game_entity_id_type targetId)
        : _mainTargetId(targetId)
    {
    }

    bool MainTargetSelector::SelectEntityId(const GameInstance& gameInstance)
    {
        return gameInstance.GetEntityViewContainer().Has(_mainTargetId);
    }

    auto MainTargetSelector::GetTargetId() const -> std::span<const game_entity_id_type>
    {
        return std::span(&_mainTargetId, 1);
    }

    bool MainTargetSelector::SelectEntity(const GameInstance& gameInstance)
    {
        _entity = gameInstance.GetEntityContainer().Find(_mainTargetId);

        return _entity.operator bool();
    }

    auto MainTargetSelector::GetTarget() const -> target_type
    {
        return _entity.get();
    }
}
