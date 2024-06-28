#include "player_move.h"

#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/view/game_view_controller.h"

namespace zerosugar::xr::game_task
{
    PlayerMove::PlayerMove(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId, std::chrono::system_clock::time_point creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
    {
    }

    void PlayerMove::Execute(GameExecutionParallel& parallel, MainTargetSelector::target_type target)
    {
        (void)parallel;

        _id = target->GetId();

        const network::game::cs::MovePlayer& param = GetParam();
        _newPosition.x() = param.position.x;
        _newPosition.y() = param.position.y;
        _newPosition.z() = param.position.z;

        target->GetComponent<MovementComponent>().SetPosition(_newPosition);
    }

    void PlayerMove::OnComplete(GameExecutionSerial& serial)
    {
        serial.GetViewController().ProcessMovement(_id, _newPosition);
    }
}
