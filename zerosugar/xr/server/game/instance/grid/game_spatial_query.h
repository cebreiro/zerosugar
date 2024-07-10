#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameSector;
    class GameSpatialContainer;
    class GameSnapshotContainer;
    class NavigationService;
}

namespace zerosugar::xr
{
    class GameSpatialQuery
    {
    public:
        GameSpatialQuery(const GameSpatialContainer& spatialContainer, const GameSnapshotContainer& snapshotContainer,
            NavigationService* navigationService);

        bool operator()(const Eigen::Vector3d& center, const Eigen::AlignedBox2d& box, float yaw, GameEntityType targetType,
            const std::function<void(game_entity_id_type)>& callback) const;

    private:
        const GameSpatialContainer& _spatialContainer;
        const GameSnapshotContainer& _snapshotContainer;
        NavigationService* _navigationService;
    };
}
