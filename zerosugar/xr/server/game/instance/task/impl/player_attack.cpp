#include "player_attack.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::game_task
{
    PlayerAttack::PlayerAttack(UniquePtrNotNull<network::game::cs::StartPlayerAttack> param, game_entity_id_type targetId,
        game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
    {
    }

    void PlayerAttack::Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type player)
    {
        (void)parallelContext;

        const network::game::cs::StartPlayerAttack& attack = GetParam();

        MovementComponent& movementComponent = player->GetComponent<MovementComponent>();
        movementComponent.SetPosition(Eigen::Vector3d(attack.position.x, attack.position.y, attack.position.y));
        movementComponent.SetYaw(attack.rotation.yaw);

        _resultPosition = movementComponent.GetPosition();
        _resultRotation.y() = movementComponent.GetYaw();
    }

    void PlayerAttack::OnComplete(GameExecutionSerial& serialContext)
    {
        const auto selfId = GetSelector<MainTargetSelector>().GetTargetId()[0];
        const network::game::cs::StartPlayerAttack& attack = GetParam();

        serialContext.GetSnapshotController().ProcessPlayerAttack(selfId, attack.skillId, _resultPosition, _resultRotation);
    }
}
