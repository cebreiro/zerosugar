#include "game_task_target_selector.h"


#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    bool NullSelector::SelectEntityId(const GameExecutionSerial& serial)
    {
        (void)serial;

        return true;
    }

    auto NullSelector::GetTargetId() const -> std::span<const game_entity_id_type>
    {
        return std::span(&_id, 1);
    }

    bool NullSelector::SelectEntity(const GameExecutionParallel& parallel)
    {
        (void)parallel;

        return true;
    }

    auto NullSelector::GetTarget() const -> target_type
    {
        return DummyTarget{};
    }

    MainTargetSelector::MainTargetSelector(game_entity_id_type targetId)
        : _mainTargetId(targetId)
    {
    }

    bool MainTargetSelector::SelectEntityId(const GameExecutionSerial& serial)
    {
        return serial.GetSnapshotContainer().Has(_mainTargetId);
    }

    auto MainTargetSelector::GetTargetId() const -> std::span<const game_entity_id_type>
    {
        return std::span(&_mainTargetId, 1);
    }

    bool MainTargetSelector::SelectEntity(const GameExecutionParallel& parallel)
    {
        _entity = parallel.GetEntityContainer().Find(_mainTargetId);

        return _entity.operator bool();
    }

    auto MainTargetSelector::GetTarget() const -> target_type
    {
        return _entity.get();
    }
}
