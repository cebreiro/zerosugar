#include "player_roll_dodge.h"

#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"

namespace zerosugar::xr::game_task
{
    PlayerRollDodge::PlayerRollDodge(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
        game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
        , _id(targetId)
    {
    }

    void PlayerRollDodge::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type target)
    {
        (void)parallelContext;
        (void)target;

        const network::game::Rotation& rotation = GetParam().rotation;

        _rotation.x() = rotation.pitch;
        _rotation.y() = rotation.yaw;
        _rotation.z() = rotation.roll;

        //target->GetComponent<MovementComponent>().SetRollDodge(true, _rotation, GetBaseTime());
    }

    void PlayerRollDodge::OnComplete(GameExecutionSerial& serialContext)
    {
        serialContext.GetViewController().ProcessRollDodge(_id, _rotation);
    }
}
