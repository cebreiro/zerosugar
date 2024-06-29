#include "player_stop.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/view/game_view_controller.h"

namespace zerosugar::xr::game_task
{
    PlayerStop::PlayerStop(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
        std::chrono::system_clock::time_point creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
    {
    }

    void PlayerStop::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type target)
    {
        (void)parallelContext;

        const network::game::cs::StopPlayer& param = GetParam();
        _newPosition.x() = param.position.x;
        _newPosition.y() = param.position.y;
        _newPosition.z() = param.position.z;

        target->GetComponent<MovementComponent>().SetPosition(_newPosition);
    }

    void PlayerStop::OnComplete(GameExecutionSerial& serialContext)
    {
        serialContext.GetViewController().ProcessStop(_id, _newPosition);
    }
}
