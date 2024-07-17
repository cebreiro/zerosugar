#include "movement_controller.h"

#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_visualizer_interface.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_move.h"

namespace zerosugar::xr::ai
{
    MovementController::MovementController(AIController& controller)
        : _aiController(controller)
    {
    }

    bool MovementController::IsMoving() const
    {
        return !_paths.empty();
    }

    void MovementController::MoveTo(const Eigen::Vector3d& startPos, const Eigen::Vector3d& destPosition, double speed)
    {
        const std::array<const navi::FVector, 2> paths{ navi::FVector(startPos), navi::FVector(destPosition) };

        MovePath(paths, speed);
    }

    void MovementController::MovePath(const std::span<const navi::FVector>& paths, double speed)
    {
        assert(speed > 0);

        if (std::ssize(paths) < 2)
        {
            assert(false);

            return;
        }

        _paths.clear();
        _paths.reserve(paths.size());

        const Eigen::Vector3d startPos(paths[0].GetX(), paths[0].GetY(), paths[0].GetZ());

        _paths.emplace_back(startPos, game_clock_type::now());

        for (int64_t i = 1; i < std::ssize(paths); ++i)
        {
            const auto& [prevPos, prevArrivalTime] = _paths[i - 1];

            const Eigen::Vector3d position(paths[i].GetX(), paths[i].GetY(), paths[i].GetZ());

            const double distance = (position - prevPos).norm();
            const double time = distance / speed;

            const game_time_point_type arrivalTimePoint = prevArrivalTime + GetMilliFromGameSeconds(time);

            _paths.emplace_back(position, arrivalTimePoint);
        }

        _speed = speed;
        _pathIndex = 1;
        _lastSyncMovementPoint = startPos;

        if (!_running)
        {
            _running = true;

            _runFuture = Run();
        }

        if (NavigationService* navi = _aiController.GetGameInstance().GetNavigationService(); navi)
        {
            if (std::shared_ptr<navi::IVisualizer> visualizer = navi->GetVisualizer(); visualizer)
            {
                navi::vis::Lines lines;
                lines.positions.reserve(_paths.size());

                std::ranges::copy(_paths | std::views::elements<0>, std::back_inserter(lines.positions));

                navi::vis::Object object;
                object.shape = std::move(lines);
                object.drawColor = navi::vis::DrawColor::Yellow;

                visualizer->Draw(std::move(object), std::chrono::seconds(1));
            }
        }
    }

    void MovementController::StopMovement()
    {
        _paths.clear();
    }

    void MovementController::Shutdown()
    {
        _shutdown = true;
    }

    auto MovementController::Join() -> Future<void>
    {
        if (_running)
        {
            co_await _runFuture;
        }
    }

    auto MovementController::GetLastMovementEndTimePoint() const -> game_time_point_type
    {
        return _lastMovementEndTimePoint;
    }

    auto MovementController::Run() -> Future<void>
    {
        constexpr auto pointMovementSyncInterval = std::chrono::milliseconds(250);

        while (true)
        {
            if (_pathIndex >= std::ssize(_paths))
            {
                break;
            }

            auto futureTimePoint = game_clock_type::now() + pointMovementSyncInterval;

            int64_t resultIndex = 0;
            const Eigen::Vector3d& position = CalculatePointOnPath(futureTimePoint, _pathIndex, &resultIndex);
            _pathIndex = resultIndex;

            game_task::MonsterMoveContext context;
            context.monsterId = _aiController.GetEntityId();
            context.position = position;
            context.timePoint = futureTimePoint;

            bool endOfMovement = position == _paths.back().first;
            if (!endOfMovement)
            {
                // point interpolation
                auto& [nextPosition, nextTimePoint] = context.next.emplace();

                nextTimePoint = futureTimePoint + pointMovementSyncInterval;
                nextPosition = CalculatePointOnPath(nextTimePoint, resultIndex);
            }

            _aiController.GetGameInstance().Summit(
                std::make_unique<game_task::MonsterMove>(context),_aiController.GetId());

            if (position == _paths.back().first)
            {
                break;
            }

            co_await Delay(pointMovementSyncInterval);
            assert(ExecutionContext::IsEqualTo(_aiController.GetGameInstance().GetStrand()));

            if (_shutdown || _paths.empty())
            {
                break;
            }
        }

        _running = false;
        _paths.clear();
        _pathIndex = 0;
        _lastMovementEndTimePoint = game_clock_type::now();

        co_return;
    }

    auto MovementController::CalculatePointOnPath(game_time_point_type timePoint, std::optional<int64_t> indexHint, int64_t* resultIndex) const
        -> Eigen::Vector3d
    {
        int64_t i = indexHint.value_or(1);
        assert(i >= 1);
        assert(std::ssize(_paths) >= 2);

        while (true)
        {
            if (timePoint > _paths[i].second)
            {
                if (i == std::ssize(_paths) - 1)
                {
                    break;
                }

                ++i;
            }
            else
            {
                break;
            }
        }

        const auto& [startPos, startTimePoint] = _paths[i - 1];
        const auto& [endPos, arrivalTimePoint] = _paths[i];

        assert(timePoint >= startTimePoint);
        assert(arrivalTimePoint >= startTimePoint);

        const double numerator = static_cast<double>((timePoint - startTimePoint).count());
        const double denominator = static_cast<double>((arrivalTimePoint - startTimePoint).count());

        const double t = std::clamp(numerator / denominator, 0.0, 1.0);

        if (resultIndex)
        {
            *resultIndex = i;
        }

        return ((1.0 - t) * startPos) + (t * endPos);
    }
}
