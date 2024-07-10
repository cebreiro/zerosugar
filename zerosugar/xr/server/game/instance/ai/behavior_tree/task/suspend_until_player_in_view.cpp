#include "suspend_until_player_in_view.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_monster_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_sector.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"

namespace zerosugar::xr::ai
{
    auto SuspendUntilPlayerInView::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        const GameInstance& gameInstance = controller.GetGameInstance();
        const GameSnapshotContainer& snapshotContainer = gameInstance.GetSnapshotContainer();
        const GameSpatialContainer& spatialContainer = gameInstance.GetSpatialContainer();

        const GameMonsterSnapshot* monster = snapshotContainer.FindMonster(controller.GetEntityId());
        assert(monster);

        const auto& position = monster->GetPosition();
        const GameSpatialSector& sector = spatialContainer.GetSector(position.x(), position.y());

        if (sector.HasEntitiesAtLeast(GameEntityType::Player, 1))
        {
            co_return true;
        }

        co_await bt::Event<network::game::sc::AddRemotePlayer>();

        co_return true;
    }

    auto SuspendUntilPlayerInView::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SuspendUntilPlayerInView&, const pugi::xml_node&)
    {
    }
}
