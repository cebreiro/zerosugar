#include "use_random_skill.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/data/table/monster.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"
#include "zerosugar/xr/server/game/instance/ai/movement/movement_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_attack.h"

namespace zerosugar::xr::ai
{
    auto UseRandomSkill::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        const GamePlayerSnapshot* target = controller.GetAggroContainer().GetPrimaryTarget();
        if (!target)
        {
            co_return false;
        }

        GameMonsterSnapshot* monster = controller.GetGameInstance().GetSnapshotContainer().FindMonster(controller.GetEntityId());
        assert(monster);

        const auto [skill, animation] = [&]() -> std::pair<const data::MonsterSkill*, const data::MonsterAnimation::Value*>
            {
                const MonsterData& data = monster->GetData();

                const auto skills = data.GetSkills();
                const int64_t count = std::ranges::distance(skills);

                if (count == 0)
                {
                    return { nullptr, nullptr };
                }

                const int64_t index = controller.GetRandomEngine()() % count;
                auto iter = skills.begin();

                const data::MonsterSkill& skillData = *std::next(iter, index);
                const data::MonsterAnimation::Value* animationData = data.FindAnimation(skillData.name);

                if (!animationData)
                {
                    return { nullptr, nullptr };
                }

                return { &skillData, animationData };
            }();

        if (!skill || !animation)
        {
            assert(false);

            co_return false;
        }
        
        MovementController& movementController = controller.GetMovementController();
        movementController.StopMovement();

        const Eigen::Vector3d diff = (target->GetPosition() - monster->GetPosition());

        const double yaw = std::atan2(diff.y(), diff.x()) * 180.0 / std::numbers::pi;

        game_task::MonsterAttackContext context;
        context.yaw = static_cast<float>(yaw);
        context.skill = skill;
        context.animation = animation;

        controller.GetGameInstance().Summit(
            std::make_unique<game_task::MonsterAttack>(controller.GetEntityId(), context),
            controller.GetControllerId());

        struct SkillEndTime{};
        const int64_t eventCounter = controller.PublishEventCounter();

        Delay(GetMilliFromGameSeconds(animation->duration))
            .Then(controller.GetGameInstance().GetStrand(),
                [weak = controller.weak_from_this(), eventCounter]()
                {
                    const auto controller = weak.lock();
                    if (!controller || controller->HasDifferenceEventCounter(eventCounter))
                    {
                        return;
                    }

                    controller->InvokeOnBehaviorTree([](BehaviorTree& bt)
                        {
                            assert(bt.IsWaitFor<SkillEndTime>());

                            bt.Notify(SkillEndTime{});
                        });
                });

        co_await bt::Event<SkillEndTime>();

        co_return true;
    }

    auto UseRandomSkill::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(UseRandomSkill&, const pugi::xml_node&)
    {
    }
}
