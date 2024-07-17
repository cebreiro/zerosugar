#include "movement_component.h"

#include <numbers>

namespace zerosugar::xr
{
    bool MovementComponent::IsMoving(game_time_point_type current) const
    {
        return std::visit([current]<typename T>(const T& movement) -> bool
            {
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    return false;
                }
                else if constexpr (std::is_same_v<T, PointMovement>)
                {
                    if (movement.next.has_value())
                    {
                        return true;
                    }

                    return current >= movement.endTimePoint;
                }
                else if constexpr (std::is_same_v<T, ForwardMovement>)
                {
                    if (current < movement.startTimePoint || current >= movement.endTimePoint)
                    {
                        return false;
                    }

                    return true;
                }
                else
                {
                    static_assert(!sizeof(T), "not implemented");

                    return false;
                }
            }, _movement);
    }

    void MovementComponent::Update(game_time_point_type current)
    {
        if (_updateTimePoint == current)
        {
            return;
        }

        _updateTimePoint = current;

        const bool complete = std::visit([current, this]<typename T>(T& movement) -> bool
        {
            if constexpr (std::is_same_v<T, std::monostate>)
            {
                return false;
            }
            else if constexpr (std::is_same_v<T, PointMovement>)
            {
                const Eigen::Vector3d* destPosition = nullptr;
                const game_time_point_type* endTimePoint = nullptr;

                if (movement.next)
                {
                    destPosition = &movement.next->first;
                    endTimePoint = &movement.next->second;
                }
                else
                {
                    destPosition = &movement.destPosition;
                    endTimePoint = &movement.endTimePoint;
                }

                const Eigen::Vector3d prevPos = _position;

                _position = Lerp(prevPos, *destPosition, current,
                    movement.lastUpdateTimePoint, *endTimePoint);

                const Eigen::Vector3d diff = (_position - prevPos);

                _yaw = std::atan2(diff.y(), diff.x()) / std::numbers::pi * 180.0;
                movement.lastUpdateTimePoint = current;

                return current >= movement.endTimePoint;
            }
            else if constexpr (std::is_same_v<T, ForwardMovement>)
            {
                if (current >= movement.endTimePoint)
                {
                    _position = movement.destPosition;

                    return true;
                }
                else if (current < movement.startTimePoint)
                {
                    assert(false);

                    return true;
                }

                _position = Lerp(movement.startPosition, movement.destPosition, current,
                    movement.startTimePoint, movement.endTimePoint);

                return false;
            }
            else
            {
                static_assert(!sizeof(T), "not implemented");
                return true;
            }

        }, _movement);

        if (complete)
        {
            _movement = std::monostate{};
        }
    }

    void MovementComponent::UpdatePointMovement(const Eigen::Vector3d& destPosition, game_time_point_type endTimePoint,
        const std::optional<std::pair<Eigen::Vector3d, game_time_point_type>>& next, game_time_point_type current)
    {
        if (PointMovement* pointMovement = std::get_if<PointMovement>(&_movement); pointMovement)
        {
            pointMovement->destPosition = destPosition;
            pointMovement->endTimePoint = endTimePoint;
            pointMovement->next = next;
        }
        else
        {
            PointMovement temp;
            temp.lastUpdateTimePoint = _updateTimePoint;
            temp.destPosition = destPosition;
            temp.endTimePoint = endTimePoint;
            temp.next = next;

            _movement = temp;
        }

        Update(current);
    }

    auto MovementComponent::StartForwardMovement(double yaw, double distance, double duration, game_time_point_type current)
        -> Eigen::Vector3d
    {
        _yaw = yaw;

        ForwardMovement forwardMovement;

        const Eigen::AngleAxisd axis(yaw * std::numbers::pi / 180.0, Eigen::Vector3d::UnitZ());
        const Eigen::Vector3d destPos = _position + (axis.toRotationMatrix() * Eigen::Vector3d(distance, 0, 0));

        forwardMovement.startTimePoint = current;
        forwardMovement.endTimePoint = current + GetMilliFromGameSeconds(duration);
        forwardMovement.startPosition = _position;
        forwardMovement.destPosition = destPos;

        _movement = forwardMovement;

        return destPos;
    }

    auto MovementComponent::GetPosition(game_time_point_type current) const -> const Eigen::Vector3d&
    {
        const_cast<MovementComponent*>(this)->Update(current);

        return _position;
    }

    auto MovementComponent::GetX() const -> double
    {
        return GetPosition(GameExecutionParallel::GetBaseTimePoint()).x();
    }

    auto MovementComponent::GetY() const -> double
    {
        return GetPosition(GameExecutionParallel::GetBaseTimePoint()).z();
    }

    auto MovementComponent::GetZ() const -> double
    {
        return GetPosition(GameExecutionParallel::GetBaseTimePoint()).z();
    }

    auto MovementComponent::GetYaw() const -> double
    {
        return _yaw;
    }

    void MovementComponent::SetPosition(const Eigen::Vector3d& position)
    {
        _position = position;

        _movement = std::monostate{};
    }

    void MovementComponent::SetYaw(float value)
    {
        _yaw = value;
    }

    auto MovementComponent::Lerp(const Eigen::Vector3d& startPos, const Eigen::Vector3d& endPos,
                                 game_time_point_type timePoint,  game_time_point_type startTimePoint, game_time_point_type endTimePoint)
        -> Eigen::Vector3d
    {
        const auto convert = []<typename T>(const T & duration) -> std::chrono::duration<double, std::chrono::nanoseconds::period>
        {
            return std::chrono::duration_cast<
                std::chrono::duration<double, std::chrono::nanoseconds::period>>(duration);
        };

        const double numerator = convert(timePoint - startTimePoint).count();
        const double denominator = convert(endTimePoint - startTimePoint).count();

        const double t = std::clamp(numerator / denominator, 0.0, 1.0);

        return ((1.0 - t) * startPos) + (t * endPos);
    }
}
