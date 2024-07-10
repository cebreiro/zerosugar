#include "gm_visualize_field.h"

#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
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
        if (!navigationService || navigationService->IsRunningVisualizer())
        {
            return false;
        }

        const GameSnapshotContainer& snapshotContainer = serialContext.GetSnapshotContainer();
        auto playerRange = snapshotContainer.GetPlayerRange();
        auto monsterRange = snapshotContainer.GetMonsterRange();

        std::vector<navi::AddVisualizeTargetParam> params;
        params.reserve(std::ranges::distance(playerRange) + std::ranges::distance(monsterRange));

        for (const std::unique_ptr<GamePlayerSnapshot>& snapshot : snapshotContainer.GetPlayerRange())
        {
            navi::AddVisualizeTargetParam& param = params.emplace_back();

            param.id = snapshot->GetId().Unwrap();
            param.position = snapshot->GetPosition();
            param.radius = game_constant::player_radius;
            param.color = navi::DrawColor::Green;
        }

        for (const std::unique_ptr<GameMonsterSnapshot>& snapshot : snapshotContainer.GetMonsterRange())
        {
            navi::AddVisualizeTargetParam& param = params.emplace_back();

            param.id = snapshot->GetId().Unwrap();
            param.position = snapshot->GetPosition();
            param.radius = 45.f;
            param.color = navi::DrawColor::Red;
        }

        using namespace network::game;

        const std::string& observerKey = name;

        navigationService->StartVisualize([holder = serialContext.Hold(), context = &serialContext, observerKey]()
            {
                GameSnapshotView& view = context->GetSnapshotView();

                view.RemoveObserver<sc::EnterGame>(observerKey);
                // TODO: leave game
                view.RemoveObserver<sc::MoveRemotePlayer>(observerKey);
                view.RemoveObserver<sc::StopRemotePlayer>(observerKey);

                view.RemoveObserver<sc::SpawnMonster>(observerKey);
                view.RemoveObserver<sc::AttackMonster>(observerKey);
                view.RemoveObserver<sc::MoveMonster>(observerKey);
                // TODO: monster despawn, movement
            });
        navigationService->AddDrawTargets(std::move(params));

        GameSnapshotView& view = serialContext.GetSnapshotView();
        auto navi = navigationService->shared_from_this();

        view.AddObserver<sc::EnterGame>(observerKey, [navi](const sc::EnterGame& packet)
            {
                const auto& pos = packet.localPlayer.transform.position;

                navi::AddVisualizeTargetParam param;
                param.id = packet.localPlayer.id;
                param.position = Eigen::Vector3d(pos.x, pos.y, pos.z);
                param.radius = game_constant::player_radius;
                param.color = navi::DrawColor::Green;

                navi->AddDrawTarget(std::move(param));
            });

        view.AddObserver<sc::MoveRemotePlayer>(observerKey, [navi](const sc::MoveRemotePlayer& packet)
            {
                const auto& pos = packet.position;

                navi::UpdateVisualizeTargetParam param;
                param.id = packet.id;
                param.position = Eigen::Vector3d(pos.x, pos.y, pos.z);

                navi->UpdateDrawTarget(std::move(param));
            });

        view.AddObserver<sc::StopRemotePlayer>(observerKey, [navi](const sc::StopRemotePlayer& packet)
            {
                const auto& pos = packet.position;

                navi::UpdateVisualizeTargetParam param;
                param.id = packet.id;
                param.position = Eigen::Vector3d(pos.x, pos.y, pos.z);

                navi->UpdateDrawTarget(std::move(param));
            });

        view.AddObserver<sc::SpawnMonster>(observerKey, [navi](const sc::SpawnMonster& packet)
            {
                for (const Monster& monster : packet.monsters)
                {
                    const auto& pos = monster.transform.position;

                    navi::AddVisualizeTargetParam param;
                    param.id = monster.id;
                    param.position = Eigen::Vector3d(pos.x, pos.y, pos.z);
                    param.radius = game_constant::player_radius;
                    param.color = navi::DrawColor::Red;

                    navi->AddDrawTarget(std::move(param));
                }
            });

        view.AddObserver<sc::AttackMonster>(observerKey, [navi](const sc::AttackMonster& packet)
            {
                const auto& pos = packet.position;
                const auto& destPos = packet.destPosition;

                navi::UpdateVisualizeTargetParam param;
                param.id = packet.id;
                param.position = Eigen::Vector3d(pos.x, pos.y, pos.z);
                param.destPosition = Eigen::Vector3d(destPos.x, destPos.y, destPos.z);
                param.destMovementDuration = packet.destMovementDuration;
                param.destPositionDrawColor = navi::DrawColor::Yellow;

                navi->UpdateDrawTarget(std::move(param));
            });

        view.AddObserver<sc::MoveMonster>(observerKey, [navi](const sc::MoveMonster& packet)
            {
                const auto& pos = packet.position;

                // TODO: print yaw
                (void)packet.rotation;

                navi::UpdateVisualizeTargetParam param;
                param.id = packet.id;
                param.position = Eigen::Vector3d(pos.x, pos.y, pos.z);

                navi->UpdateDrawTarget(std::move(param));
            });

        return true;
    }
}
