#include "bot_movement_controller.h"

#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/network/packet.h"

namespace zerosugar::xr::bot
{
    MovementController::MovementController(BotController& controller)
        : _botController(controller)
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

            const game_time_point_type arrivalTimePoint = prevArrivalTime + std::chrono::duration_cast<
                std::chrono::milliseconds>(std::chrono::duration<double, std::chrono::seconds::period>(time));

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
    }

    void MovementController::StopMovement()
    {
        _paths.clear();

        _lastMovementEndTimePoint = std::chrono::system_clock::now();
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
        constexpr auto moveInterval = std::chrono::milliseconds(250);

        while (true)
        {
            if (_pathIndex >= std::ssize(_paths))
            {
                break;
            }

            auto now = std::chrono::system_clock::now();

            int64_t resultIndex = 0;
            const Eigen::Vector3d& position = CalculatePointOnPath(now, _pathIndex, &resultIndex);
            _pathIndex = resultIndex;

            LocalPlayer& localPlayer = _botController.GetLocalPlayer();

            const Eigen::Vector3d diff = (position - localPlayer.GetPosition());
            const double yaw = std::atan2(diff.y(), diff.x());

            Eigen::Vector3d rotation = localPlayer.GetRotation();
            rotation.y() = yaw;

            localPlayer.SetPosition(position);
            localPlayer.SetRotation(rotation);

            network::game::cs::MovePlayer packet;
            packet.position.x = static_cast<float>(position.x());
            packet.position.y = static_cast<float>(position.y());
            packet.position.z = static_cast<float>(position.z());
            packet.rotation.pitch = static_cast<float>(rotation.x());
            packet.rotation.yaw = static_cast<float>(rotation.y());
            packet.rotation.roll = static_cast<float>(rotation.z());

            _botController.SendToServer(Packet::ToBuffer(packet));

            if (position == _paths.back().first)
            {
                break;
            }

            co_await Delay(moveInterval);

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
