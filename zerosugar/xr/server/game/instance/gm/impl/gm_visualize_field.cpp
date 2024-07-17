#include "gm_visualize_field.h"

#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_visualize_param.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/data/game_data_constant.h"
#include "zerosugar/xr/navigation/navi_visualizer.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"

namespace zerosugar::xr::gm
{
    bool VisualizeField::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player)
    {
        (void)player;

        NavigationService* navigationService = serialContext.GetNavigationService();
        if (!navigationService)
        {
            return false;
        }

        using namespace network::game;

        const std::string& observerKey = name;

        Future<bool> future = navigationService->StartVisualize(
            [serial = serialContext.Hold(), context = &serialContext, observerKey]()
            {
                Post(serial->GetStrand(), [serial, context, observerKey]()
                {
                    GameSnapshotView& view = context->GetSnapshotView();

                    view.RemoveObserver<sc::EnterGame>(observerKey);
                    // TODO: leave game
                    view.RemoveObserver<sc::MoveRemotePlayer>(observerKey);
                    view.RemoveObserver<sc::StopRemotePlayer>(observerKey);

                    view.RemoveObserver<sc::SpawnMonster>(observerKey);
                    view.RemoveObserver<sc::AttackMonster>(observerKey);
                    view.RemoveObserver<sc::MoveMonster>(observerKey);
                    view.RemoveObserver<sc::DespawnMonster>(observerKey);
                });
            });

        // temp...
        const bool startVisualizer = future.Get();
        if (!startVisualizer)
        {
            return false;
        }

        const std::shared_ptr<navi::IVisualizer> visualizer = navigationService->GetVisualizer();
        assert(visualizer);

        const GameSnapshotContainer& snapshotContainer = serialContext.GetSnapshotContainer();
        
        for (const std::unique_ptr<GamePlayerSnapshot>& snapshot : snapshotContainer.GetPlayerRange())
        {
            navi::vis::Agent agent{
                .position = snapshot->GetPosition(),
                .radius = game_constant::player_radius,
                .drawColor = navi::vis::DrawColor::Green,
            };

            (void)visualizer->AddAgent(snapshot->GetId().Unwrap(), std::move(agent));
        }

        for (const std::unique_ptr<GameMonsterSnapshot>& snapshot : snapshotContainer.GetMonsterRange())
        {
            navi::vis::Agent agent{
                .position = snapshot->GetPosition(),
                .radius = game_constant::player_radius,
                .drawColor = navi::vis::DrawColor::Red,
            };

            (void)visualizer->AddAgent(snapshot->GetId().Unwrap(), std::move(agent));
        }

        GameSnapshotView& view = serialContext.GetSnapshotView();
        auto navi = navigationService->shared_from_this();

        view.AddObserver<sc::EnterGame>(observerKey, [visualizer](const sc::EnterGame& packet)
            {
                const auto& pos = packet.localPlayer.transform.position;

                navi::vis::Agent agent{
                    .position = Eigen::Vector3d(pos.x, pos.y, pos.z),
                    .radius = game_constant::player_radius,
                    .drawColor = navi::vis::DrawColor::Green,
                };

                (void)visualizer->AddAgent(packet.localPlayer.id, std::move(agent));
            });

        view.AddObserver<sc::MoveRemotePlayer>(observerKey, [visualizer](const sc::MoveRemotePlayer& packet)
            {
                (void)visualizer->UpdateAgentPositionAndYaw(packet.id,
                    Eigen::Vector3d(packet.position.x, packet.position.y, packet.position.z), packet.rotation.yaw);
            });

        view.AddObserver<sc::StopRemotePlayer>(observerKey, [visualizer](const sc::StopRemotePlayer& packet)
            {
                (void)visualizer->UpdateAgentPosition(packet.id,
                    Eigen::Vector3d(packet.position.x, packet.position.y, packet.position.z));
            });

        view.AddObserver<sc::RemotePlayerAttack>(observerKey, [visualizer, navi](const sc::RemotePlayerAttack& packet)
            {
                const Eigen::Vector3d pos(packet.position.x, packet.position.y, packet.position.z);

                (void)visualizer->UpdateAgentPositionAndYaw(packet.id, pos, packet.rotation.yaw);

                const std::array<PlayerAttack, 4>& attack = DataConstant::GetInstance().GetPlayerAttacks();

                const int64_t index = packet.motionId - 1;

                if (index < 0 || index >= std::ssize(attack))
                {
                    return;
                }

                const PlayerAttack& attackData = attack[index];
                const navi::vis::Object object{
                    .shape =
                        std::visit([&pos, &packet]<typename T>(const T & range) -> navi::vis::Object::shape_type
                        {
                            if constexpr (std::is_same_v<T, Eigen::AlignedBox3d>)
                            {
                                const Eigen::AlignedBox3d& box = range;

                                const Eigen::AngleAxisd axis(packet.rotation.yaw * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());
                                const auto rotation = axis.toRotationMatrix();

                                return navi::vis::OBB{
                                    .center = pos + rotation * box.center(),
                                    .halfSize = box.sizes() * 0.5,
                                    .rotation = rotation,
                                };
                            }
                            else if constexpr (std::is_same_v<T, double>)
                            {
                                return navi::vis::Circle{
                                    .center = pos,
                                    .radius = range,
                                };
                            }
                            else
                            {
                                static_assert(!sizeof(T), "not implemented");

                                return {};
                            }
                        }, attackData.attackRange),
                    .drawColor = navi::vis::DrawColor::White,
                };

                for (const double delay : attackData.attackEffectDelay)
                {
                    const auto displayDelay = GetMilliFromGameSeconds(delay);
                    const auto displayDuration = GetMilliFromGameSeconds(attackData.duration) - displayDelay;

                    Delay(displayDelay)
                        .Then(navi->GetStrand(), [visualizer, object, displayDuration]()
                            {
                                (void)visualizer->Draw(object, displayDuration);
                            });
                }
            });

        view.AddObserver<sc::SpawnMonster>(observerKey, [visualizer](const sc::SpawnMonster& packet)
            {
                for (const Monster& monster : packet.monsters)
                {
                    const auto& pos = monster.transform.position;

                    navi::vis::Agent agent{
                        .position = Eigen::Vector3d(pos.x, pos.y, pos.z),
                        .radius = game_constant::player_radius,
                        .drawColor = navi::vis::DrawColor::Red,
                    };

                    (void)visualizer->AddAgent(monster.id, std::move(agent));
                }
            });

        view.AddObserver<sc::AttackMonster>(observerKey, [visualizer](const sc::AttackMonster& packet)
            {
                const Eigen::Vector3d position(packet.position.x, packet.position.y, packet.position.z);

                (void)visualizer->UpdateAgentPositionAndYaw(packet.id, position, packet.rotation.yaw);

                if (packet.destMovementDuration <= 0.0)
                {
                    return;
                }

                navi::vis::Agent::Movement movement{
                    .destPosition = Eigen::Vector3d(packet.destPosition.x, packet.destPosition.y, packet.destPosition.z),
                    .startTimePoint = std::chrono::system_clock::now(),
                    .duration = packet.destMovementDuration,
                    .drawColor = navi::vis::DrawColor::Yellow,
                };

                (void)visualizer->UpdateAgentMovement(packet.id, position, std::move(movement));
            });

        view.AddObserver<sc::MoveMonster>(observerKey, [visualizer](const sc::MoveMonster& packet)
            {
                (void)visualizer->UpdateAgentPositionAndYaw(packet.id,
                    Eigen::Vector3d(packet.position.x, packet.position.y, packet.position.z),
                    packet.rotation.yaw);
            });

        view.AddObserver<sc::DespawnMonster>(observerKey, [visualizer](const sc::DespawnMonster& packet)
            {
                for (int64_t monsterId : packet.monsters)
                {
                    (void)visualizer->RemoveAgent(monsterId);
                }
            });

        return true;
    }
}
