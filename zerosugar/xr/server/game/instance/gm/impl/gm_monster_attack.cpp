#include "gm_monster_attack.h"

#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_attack.h"

namespace zerosugar::xr::gm
{
    bool MonsterAttack::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player,
        const std::optional<int32_t>& skillId)
    {
        const GameSnapshotContainer& snapshotContainer = serialContext.GetSnapshotContainer();
        const auto& position = player.GetPosition();

        auto& sector = serialContext.GetSpatialContainer().GetSector(position);

        const Eigen::Vector2d pos2d(position.x(), position.y());

        std::map<double, game_entity_id_type> sorted;

        for (game_entity_id_type monsterId : sector.GetEntities(GameEntityType::Monster))
        {
            const std::optional<Eigen::Vector3d> targetPos = snapshotContainer.FindPosition(monsterId);
            assert(targetPos.has_value());

            const Eigen::Vector2d targetPos2d(targetPos->x(), targetPos->y());

            sorted.emplace((pos2d - targetPos2d).squaredNorm(), monsterId);
        }

        if (sorted.empty())
        {
            return false;
        }

        const GameMonsterSnapshot* monster = snapshotContainer.FindMonster(sorted.begin()->second);
        assert(monster);

        game_task::MonsterAttackContext context;

        if (skillId.has_value())
        {
            context.skill = monster->GetData().FindSkill(*skillId);
            if (!context.skill)
            {
                return false;
            }

            context.animation = monster->GetData().FindAnimation(context.skill->name);
        }
        else
        {
            const auto skills = monster->GetData().GetSkills();
            const int64_t count = std::ranges::distance(skills);

            if (count == 0)
            {
                return false;
            }

            const int64_t index = std::random_device{}() % count;
            auto iter = skills.begin();

            const data::MonsterSkill& skill = *std::next(iter, index);

            context.skill = &skill;
            context.animation = monster->GetData().FindAnimation(skill.name);
        }

        if (!context.animation)
        {
            return false;
        }

        serialContext.SummitTask(std::make_unique<game_task::MonsterAttack>(
            monster->GetId(), context));

        return true;
    }
}
