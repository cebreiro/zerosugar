#include "attack_target.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/collision/shape/obb.h"
#include "zerosugar/shared/collision/shape/circle.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/collision/intersect.h"
#include "zerosugar/xr/data/game_data_constant.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/movement/bot_movement_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/monster.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/visual_object_container.h"
#include "zerosugar/xr/network/packet.h"

namespace zerosugar::xr::bot
{
    using shape_type = std::variant<collision::OBB, collision::Circle>;

    auto GetBox(int32_t combo) -> Eigen::AlignedBox3d
    {
        if (combo < 3)
        {
            return Eigen::AlignedBox3d(
                Eigen::Vector3d(0, -120, 0),
                Eigen::Vector3d(600.f, 120.f, 100.f));
        }

        return Eigen::AlignedBox3d(
            Eigen::Vector3d(0, -200, 0),
            Eigen::Vector3d(600.f, 200.f, 100.f));
    }

    auto MakeCollisionShape(const Eigen::Vector3d& position, float yaw, int32_t combo) -> shape_type
    {
        switch (combo)
        {
        case 1:
        case 2:
        case 3:
        {
            const Eigen::AngleAxisd axis(yaw * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());
            const auto rotation = axis.toRotationMatrix();

            const Eigen::AlignedBox3d box = GetBox(combo);
            const auto localCenter = rotation * box.center();

            return collision::OBB3d(position + localCenter, box.sizes() * 0.5, rotation).Project();
        }
        case 4:
            return collision::Circle(Eigen::Vector2d(position.x(), position.y()), 400.0);
        }

        assert(false);

        return collision::Circle{};
    }

    auto AttackTarget::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();

        auto battleTarget = blackBoard.GetOr<std::shared_ptr<Monster>>("battle_target", nullptr);
        if (!battleTarget)
        {
            co_return false;
        }

        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);
        controller.GetMovementController().StopMovement();

        LocalPlayer& player = controller.GetLocalPlayer();

        const std::array<PlayerAttack, 4>& attacks = DataConstant::GetInstance().GetPlayerAttacks();
        int32_t comboCount = 1;

        while (comboCount <= 4)
        {
            auto target = controller.GetVisualObjectContainer().FindMonster(battleTarget->GetData().id);
            if (!target)
            {
                co_return false;
            }

            const PlayerAttack& data = attacks[comboCount - 1];

            const auto diff = target->GetPosition() - player.GetPosition();
            const float yaw = static_cast<float>(std::atan2(diff.y(), diff.x()) * 180.0 / std::numbers::pi);

            network::game::cs::StartPlayerAttack startPlayerAttack;
            startPlayerAttack.skillId = comboCount;
            startPlayerAttack.position.x = static_cast<float>(player.GetPosition().x());
            startPlayerAttack.position.y = static_cast<float>(player.GetPosition().y());
            startPlayerAttack.position.z = static_cast<float>(player.GetPosition().z());
            startPlayerAttack.rotation.yaw = yaw;

            controller.SendToServer(Packet::ToBuffer(startPlayerAttack));

            const std::chrono::milliseconds skillDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::duration<double, std::chrono::seconds::period>(data.duration));

            player.SetRotation(Eigen::Vector3d(0.0, yaw, 0.0));
            player.SetAttack(comboCount, std::chrono::system_clock::now() + skillDuration);

            bool hitMonster = false;

            for (const auto attackEffectDelay : data.attackEffectDelay)
            {
                const auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::duration<double, std::chrono::seconds::period>(attackEffectDelay));

                Delay(milli).Then(controller.GetStrand(),
                    [&hitMonster, weak = controller.weak_from_this(), zoneId = player.GetZoneId(), startPlayerAttack, shape = MakeCollisionShape(player.GetPosition(), yaw, comboCount)]()
                    {
                        const auto controller = weak.lock();
                        if (!controller || controller->GetLocalPlayer().GetZoneId() != zoneId)
                        {
                            return;
                        }

                        if (!controller->GetLocalPlayer().IsAttackState(startPlayerAttack.skillId, std::chrono::system_clock::now()))
                        {
                            return;
                        }

                        network::game::cs::ApplyPlayerAttack applyPlayerAttack;
                        applyPlayerAttack.id = controller->GetLocalPlayer().GetData().id;
                        
                        applyPlayerAttack.skillId = startPlayerAttack.skillId;
                        applyPlayerAttack.position = startPlayerAttack.position;
                        applyPlayerAttack.rotation = startPlayerAttack.rotation;

                        for (const Monster& monster : controller->GetVisualObjectContainer().GetMonsterRange())
                        {
                            Eigen::Vector2d monsterPos2d(monster.GetPosition().x(), monster.GetPosition().y());
                            const collision::Circle monsterCollision(monsterPos2d, 100.0);

                            if (std::visit([&monsterCollision]<typename T>(const T & shape) -> bool
                            {
                                return collision::Intersect(monsterCollision, shape);

                            }, shape))
                            {
                                ++applyPlayerAttack.targetCount;
                                applyPlayerAttack.targets.push_back(monster.GetData().id);
                            }
                        }

                        if (applyPlayerAttack.targetCount > 0)
                        {
                            controller->SendToServer(Packet::ToBuffer(applyPlayerAttack));

                            hitMonster = true;
                        }
                    });
            }

            struct SuspendUntilSkillEnd{};

            Delay(skillDuration).Then(controller.GetStrand(), [weak = controller.weak_from_this()]()
                {
                    auto controller = weak.lock();
                    if (!controller)
                    {
                        return;
                    }

                    controller->InvokeOnBehaviorTree([](BehaviorTree& bt)
                        {
                            assert(bt.IsWaitFor<SuspendUntilSkillEnd>());
                            bt.Notify(SuspendUntilSkillEnd{});
                        });
                });

            co_await bt::Event<SuspendUntilSkillEnd>();

            if (hitMonster)
            {
                ++comboCount;
            }
            else
            {
                co_return false;
            }
        }

        co_return true;
    }

    auto AttackTarget::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(AttackTarget&, const pugi::xml_node&)
    {
    }
}
