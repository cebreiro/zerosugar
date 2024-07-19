#include "game_task_target_selector.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_query.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
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
        const GameSnapshotContainer& snapshotContainer = serial.GetSnapshotContainer();

        return snapshotContainer.Has(_mainTargetId);
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

    bool MultiTargetSelector::SelectEntityId(const GameExecutionSerial& serial)
    {
        const GameSnapshotContainer& snapshotContainer = serial.GetSnapshotContainer();

        return std::ranges::all_of(_targetIds, [&snapshotContainer](game_entity_id_type id) -> bool
            {
                return snapshotContainer.FindPlayer(id) != nullptr;
            });
    }

    auto MultiTargetSelector::GetTargetId() const -> std::span<const game_entity_id_type>
    {
        return _targetIds;
    }

    bool MultiTargetSelector::SelectEntity(const GameExecutionParallel& parallel)
    {
        parallel.GetEntityContainer().FindRange(_targetIds, [this](const SharedPtrNotNull<GameEntity>& entity)
            {
                this->_targets.push_back(entity.get());
            });

        return !_targets.empty();
    }

    auto MultiTargetSelector::GetTarget() const -> target_type
    {
        return _targets;
    }

    PlayerAttackEffectTargetSelector::PlayerAttackEffectTargetSelector(game_entity_id_type playerId, std::span<const game_entity_id_type> targetIds)
        : _playerId(playerId)
        , _targetIds(targetIds.begin(), targetIds.end())
    {
        assert(!targetIds.empty());
    }

    bool PlayerAttackEffectTargetSelector::SelectEntityId(const GameExecutionSerial& serial)
    {
        const GameSnapshotContainer& snapshotContainer = serial.GetSnapshotContainer();

        const GamePlayerSnapshot* player = snapshotContainer.FindPlayer(_playerId);
        if (!player)
        {
            return false;
        }

        std::erase_if(_targetIds, [&snapshotContainer](game_entity_id_type targetId) -> bool
            {
                const GameMonsterSnapshot* target = snapshotContainer.FindMonster(targetId);
                if (!target)
                {
                    return true;
                }

                return false;
            });

        return !_targetIds.empty();
    }

    auto PlayerAttackEffectTargetSelector::GetTargetId() const -> std::span<const game_entity_id_type>
    {
        return _targetIds;
    }

    bool PlayerAttackEffectTargetSelector::SelectEntity(const GameExecutionParallel& parallel)
    {
        parallel.GetEntityContainer().FindRange(_targetIds, [this](const SharedPtrNotNull<GameEntity>& entity)
            {
                this->_targets.push_back(entity.get());
            });

        return !_targets.empty();
    }

    auto PlayerAttackEffectTargetSelector::GetTarget() const -> target_type
    {
        return _targets;
    }

    BoxSkillTargetSelector::BoxSkillTargetSelector(const Eigen::Vector3d& center, const Eigen::AlignedBox2d& box, float yaw, GameEntityType targetType)
        : _center(center)
        , _box(box)
        , _yaw(yaw)
        , _targetType(targetType)
    {
    }

    bool BoxSkillTargetSelector::SelectEntityId(const GameExecutionSerial& serial)
    {
        GameSpatialQuery query(serial.GetSpatialContainer(), serial.GetSnapshotContainer(), serial.GetNavigationService());

        const auto callback = [this](game_entity_id_type id)
            {
                _targetIds.push_back(id);
            };

        if (query(_center, _box, _yaw, _targetType, callback))
        {
            return !_targetIds.empty();
        }

        return false;
    }

    auto BoxSkillTargetSelector::GetTargetId() const -> std::span<const game_entity_id_type>
    {
        return _targetIds;
    }

    bool BoxSkillTargetSelector::SelectEntity(const GameExecutionParallel& parallel)
    {
        parallel.GetEntityContainer().FindRange(_targetIds, [this](const SharedPtrNotNull<GameEntity>& entity)
            {
                this->_targets.push_back(entity.get());
            });

        return true;
    }

    auto BoxSkillTargetSelector::GetTarget() const -> target_type
    {
        return _targets;
    }
}
