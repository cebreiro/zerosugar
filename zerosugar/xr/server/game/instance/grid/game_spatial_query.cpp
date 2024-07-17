#include "game_spatial_query.h"

#include <numbers>
#include "zerosugar/shared/collision/shape/obb.h"
#include "zerosugar/shared/collision/shape/circle.h"
#include "zerosugar/shared/collision/intersect.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_visualizer_interface.h"
#include "zerosugar/xr/server/game/instance/game_constants.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr
{
    GameSpatialQuery::GameSpatialQuery(const GameSpatialContainer& spatialContainer, const GameSnapshotContainer& snapshotContainer,
        NavigationService* navigationService)
        : _spatialContainer(spatialContainer)
        , _snapshotContainer(snapshotContainer)
        , _navigationService(navigationService)
    {
    }

    bool GameSpatialQuery::operator()(const Eigen::Vector3d& center, const Eigen::AlignedBox2d& box,
        float yaw, GameEntityType targetType, const std::function<void(game_entity_id_type)>& callback) const
    {
        assert(callback);

        Eigen::AngleAxisd axis(yaw * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());
        const Eigen::Vector2d extents = box.sizes() * 0.5;

        const Eigen::Vector3d localCenter(box.center().x(), box.center().y(), 0);
        const Eigen::Vector3d halfSize(extents.x(), extents.y(), 0);
        const Eigen::Matrix3d rotation = axis.toRotationMatrix();

        Eigen::Vector3d obbCenter = rotation * localCenter;
        obbCenter += center;

        collision::OBB3d obb3d(obbCenter, halfSize, rotation);
        collision::OBB obb = obb3d.Project();
        GameSpatialMBR mbr = GameSpatialMBR::CreateFrom(obb);

        const auto filter = [targetType , &mbr](const GameSpatialCell& cell) -> bool
            {
                return cell.HasEntity(targetType) && mbr.Intersect(cell.GetMBR());
            };

        const GameSpatialSector& sector = _spatialContainer.GetSector(center);
        for (const GameSpatialCell& cell : sector.GetCells() | std::views::filter(filter))
        {
            for (const game_entity_id_type id : cell.GetEntities(targetType))
            {
                const std::optional<Eigen::Vector3d> targetPos3d = _snapshotContainer.FindPosition(id);
                assert(targetPos3d);

                const Eigen::Vector2d targetPos(targetPos3d->x(), targetPos3d->y());
                const collision::Circle circle(targetPos, game_constant::player_radius);

                if (Intersect(circle, obb))
                {
                    callback(id);
                }
            }
        }

        if (_navigationService)
        {
            if (std::shared_ptr<navi::IVisualizer> visualizer = _navigationService->GetVisualizer())
            {
                navi::vis::Object object{
                    .shape = navi::vis::OBB{
                    .center = obbCenter,
                    .halfSize = halfSize,
                    .rotation = rotation
                    },
                    .drawColor = navi::vis::DrawColor::Yellow,
                };

                visualizer->Draw(std::move(object), std::chrono::milliseconds(2000));
            }
        }

        return true;
    }
}
