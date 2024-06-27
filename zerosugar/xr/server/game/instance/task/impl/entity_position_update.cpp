#include "entity_position_update.h"

#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_view.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_view_container.h"
#include "zerosugar/xr/server/game/instance/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"

namespace zerosugar::xr::game_task
{
    EntityPositionUpdate::EntityPositionUpdate(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId)
        : GameTaskT(std::chrono::system_clock::now(), std::move(param), MainTargetSelector(targetId))
    {
    }

    void EntityPositionUpdate::Execute(GameExecutionParallel& parallel, MainTargetSelector::target_type target)
    {
        (void)parallel;

        _id = target->GetId();

        const network::game::cs::PlayerMove& param = GetParam();
        auto& movementComponent = target->GetComponent<MovementComponent>();

        _oldPosition = movementComponent.GetPosition();

        _newPosition.x() = param.position.x;
        _newPosition.y() = param.position.y;
        _newPosition.z() = param.position.z;

        movementComponent.SetPosition(_newPosition);
    }

    void EntityPositionUpdate::OnComplete(GameExecutionSerial& serial)
    {
        GameEntityViewContainer& viewContainer = serial.GetEntityViewContainer();
        GameEntityView* view = viewContainer.Find(_id);
        if (!view)
        {
            return;
        }

        view->SetPosition(_newPosition);

        GameSpatialContainer& spatialContainer = serial.GetSpatialContainer();

        GameSpatialSector& oldSector = spatialContainer.GetSector(_oldPosition.x(), _oldPosition.y());
        GameSpatialSector& newSector = spatialContainer.GetSector(_newPosition.x(), _newPosition.y());
        {
            if (oldSector.GetId() == newSector.GetId())
            {
                return;
            }

            GameSpatialSector::View oldView = (oldSector - newSector);
            oldView.RemoveEntity(_id);

            if (auto range = oldView.GetEntities(); range.begin() != range.end())
            {
                for (const game_entity_id_type id : oldView.GetEntities())
                {
                    (void)id;
                }
            }

            GameSpatialSector::View newView = (newSector - oldSector);
            if (auto range = newView.GetEntities(); range.begin() != range.end())
            {
                for (const game_entity_id_type id : newView.GetEntities())
                {
                    (void)id;
                }
            }

            newView.AddEntity(_id);
        }

        ZEROSUGAR_LOG_DEBUG(serial.GetServiceLocator(),
            std::format("movement: old[{}, {}] new[{}, [{}], old_sector[{}, {}], new_sector[{}, {}]",
                _oldPosition.x(), _oldPosition.y(),
                _newPosition.x(), _newPosition.y(),
                oldSector.GetId().GetX(), oldSector.GetId().GetY(),
                newSector.GetId().GetX(), newSector.GetId().GetY()));
    }
}
