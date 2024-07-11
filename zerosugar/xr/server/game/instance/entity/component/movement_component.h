#pragma once
#include "zerosugar/xr/server/game/instance/game_type.h"
#include "zerosugar/xr/server/game/instance/entity/component/game_component.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"

namespace zerosugar::xr
{
    class MovementComponent : public GameComponent
    {
    public:
        bool IsMoving(game_time_point_type current = GameExecutionParallel::GetBaseTimePoint()) const;
        
        void Update(game_time_point_type current);

        void UpdatePointMovement(const Eigen::Vector3d& destPosition, game_time_point_type endTimePoint,
            const std::optional<std::pair<Eigen::Vector3d, game_time_point_type>>& next,
            game_time_point_type current = GameExecutionParallel::GetBaseTimePoint());

        auto StartForwardMovement(double yaw, double distance, double duration, game_time_point_type current = GameExecutionParallel::GetBaseTimePoint())
            -> Eigen::Vector3d;

        auto GetPosition(game_time_point_type current = GameExecutionParallel::GetBaseTimePoint()) const -> const Eigen::Vector3d&;
        
        auto GetX() const -> double;
        auto GetY() const -> double;
        auto GetZ() const -> double;
        auto GetYaw() const -> double;

        void SetPosition(const Eigen::Vector3d& position);
        void SetYaw(float value);

    private:
        static auto Lerp(const Eigen::Vector3d& startPos, const Eigen::Vector3d& endPos,
            game_time_point_type timePoint, game_time_point_type startTimePoint, game_time_point_type endTimePoint)
            -> Eigen::Vector3d;

    private:
        game_time_point_type _updateTimePoint;
        Eigen::Vector3d _position;
        double _yaw = 0.f;

        struct PointMovement
        {
            game_time_point_type lastUpdateTimePoint;

            Eigen::Vector3d destPosition;
            game_time_point_type endTimePoint;

            std::optional<std::pair<Eigen::Vector3d, game_time_point_type>> next;
        };

        struct ForwardMovement
        {
            game_time_point_type startTimePoint = {};
            game_time_point_type endTimePoint = {};
            Eigen::Vector3d startPosition;
            Eigen::Vector3d destPosition;
        };

        std::variant<std::monostate, PointMovement, ForwardMovement> _movement;
    };
}
