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
    auto Convert(const Eigen::Vector3d& vector) -> navi::FVector
    {
        const navi::FVector result(
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y()),
            static_cast<float>(vector.z()));

        return result;
    }

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
            param.position = Convert(snapshot->GetPosition());
            param.radius = game_constant::player_radius;
            param.color = navi::DrawColor::Blue;
        }

        for (const std::unique_ptr<GameMonsterSnapshot>& snapshot : snapshotContainer.GetMonsterRange())
        {
            navi::AddVisualizeTargetParam& param = params.emplace_back();

            param.id = snapshot->GetId().Unwrap();
            param.position = Convert(snapshot->GetPosition());
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
                param.position = navi::FVector(pos.x, pos.y, pos.z);
                param.radius = game_constant::player_radius;
                param.color = navi::DrawColor::Blue;

                navi->AddDrawTarget(std::move(param));
            });

        view.AddObserver<sc::MoveRemotePlayer>(observerKey, [navi](const sc::MoveRemotePlayer& packet)
            {
                const auto& pos = packet.position;

                navi::UpdateVisualizeTargetParam param;
                param.id = packet.id;
                param.position = navi::FVector(pos.x, pos.y, pos.z);

                navi->UpdateDrawTarget(std::move(param));
            });

        view.AddObserver<sc::StopRemotePlayer>(observerKey, [navi](const sc::StopRemotePlayer& packet)
            {
                const auto& pos = packet.position;

                navi::UpdateVisualizeTargetParam param;
                param.id = packet.id;
                param.position = navi::FVector(pos.x, pos.y, pos.z);

                navi->UpdateDrawTarget(std::move(param));
            });


        view.AddObserver<sc::SpawnMonster>(observerKey, [navi](const sc::SpawnMonster& packet)
            {
                for (const Monster& monster : packet.monsters)
                {
                    const auto& pos = monster.transform.position;

                    navi::AddVisualizeTargetParam param;
                    param.id = monster.id;
                    param.position = navi::FVector(pos.x, pos.y, pos.z);
                    param.radius = game_constant::player_radius;
                    param.color = navi::DrawColor::Blue;

                    navi->AddDrawTarget(std::move(param));
                }
            });

        

        return true;
    }
}
