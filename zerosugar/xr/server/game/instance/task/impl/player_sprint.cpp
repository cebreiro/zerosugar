#include "player_sprint.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/view/game_view_controller.h"

namespace zerosugar::xr::game_task
{
    PlayerSprint::PlayerSprint(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
        std::chrono::system_clock::time_point creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
        , _id(targetId)
    {
    }

    void PlayerSprint::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type target)
    {
        (void)parallelContext;
        (void)target;

        //target->GetComponent<MovementComponent>().SetSprint(true, GetBaseTime());
    }

    void PlayerSprint::OnComplete(GameExecutionSerial& serialContext)
    {
        serialContext.GetViewController().ProcessSprint(_id);
    }
}
