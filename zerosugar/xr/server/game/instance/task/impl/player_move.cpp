#include "player_move.h"

#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"

namespace zerosugar::xr::game_task
{
    PlayerMove::PlayerMove(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId, game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
    {
    }

    void PlayerMove::Execute(GameExecutionParallel& parallel, MainTargetSelector::target_type target)
    {
        (void)parallel;

        const auto& pos = GetParam().position;

        MovementComponent& movementComponent = target->GetComponent<MovementComponent>();
        movementComponent.SetPosition(Eigen::Vector3d(pos.x, pos.y, pos.z));
        movementComponent.SetYaw(GetParam().rotation.yaw);
    }

    void PlayerMove::OnComplete(GameExecutionSerial& serial)
    {
        const auto& pos = GetParam().position;
        const auto id = GetSelector<MainTargetSelector>().GetTargetId()[0];

        serial.GetSnapshotController().ProcessMovement(id,
            Eigen::Vector3d(pos.x, pos.y, pos.z),
            GetParam().rotation.yaw);
    }
}
