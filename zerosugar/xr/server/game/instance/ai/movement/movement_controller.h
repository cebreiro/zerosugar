#pragma once
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/server/game/instance/game_type.h"

namespace zerosugar::xr
{
    class AIController;
}

namespace zerosugar::xr::ai
{
    class MovementController
    {
    private:
        struct PathMovement
        {
            double speed = 0.0;
            Eigen::Vector3d destPosition = {};
        };

    public:
        explicit MovementController(AIController& controller);

        bool IsMoving() const;

        void MoveTo(const Eigen::Vector3d& startPos, const Eigen::Vector3d& destPosition, double speed);
        void MovePath(const std::span<const navi::FVector>& paths, double speed);

        void StopMovement();

        void Shutdown();
        auto Join() -> Future<void>;

        auto GetLastMovementEndTimePoint() const -> game_time_point_type;

    private:
        auto Run() -> Future<void>;

        auto CalculatePointOnPath(game_time_point_type timePoint, std::optional<int64_t> hintIndex = std::nullopt,
            int64_t* resultIndex = nullptr) const -> Eigen::Vector3d;

    private:
        AIController& _aiController;

        bool _shutdown = false;
        bool _running = false;
        Future<void> _runFuture;

        double _speed = 0.0;
        game_time_point_type _moveStartTime = game_time_point_type::min();
        std::vector<std::pair<Eigen::Vector3d, game_time_point_type>> _paths;
        int64_t _pathIndex = 0;
        Eigen::Vector3d _lastSyncMovementPoint;

        game_time_point_type _lastMovementEndTimePoint = game_time_point_type::min();
    };
}
