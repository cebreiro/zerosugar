#include "gm_item_remove.h"

#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_item_change.h"

namespace zerosugar::xr::gm
{
    bool ItemRemove::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const int32_t& slot)
    {
        game_task::ItemChange change;
        change.playerId = player.GetId();
        change.change = game_task::ItemRemove{
            .slot = slot,
        };

        auto task = std::make_unique<game_task::PlayerItemChange>(std::vector{ change });

        serialContext.SummitTask(std::move(task), player.GetController().GetControllerId());

        return true;
    }
}
