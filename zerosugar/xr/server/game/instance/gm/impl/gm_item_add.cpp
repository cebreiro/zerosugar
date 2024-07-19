#include "gm_item_add.h"

#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_item_change.h"

namespace zerosugar::xr::gm
{
    bool ItemAdd::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const int32_t& itemId, const std::optional<int32_t>& quantity)
    {
        game_task::ItemChange change;
        change.playerId = player.GetId();
        change.change = game_task::ItemAdd{
            .itemId = itemId,
            .quantity = std::max(1, quantity.value_or(1))
        };

        auto task = std::make_unique<game_task::PlayerItemChange>(std::vector{ change });

        serialContext.SummitTask(std::move(task), player.GetController().GetControllerId());

        return true;
    }
}
