#include "arc.h"

namespace zerosugar::collision
{
    Arc::Arc(Eigen::Vector2d center, double radius, double startAngle, double endAngle)
        : _center(std::move(center))
        , _radius(radius)
        , _startAngle(startAngle)
        , _endAngle(endAngle)
    {
    }

    bool Arc::Contains(double angle) const
    {
        if (angle < _startAngle || angle > _endAngle)
        {
            return false;
        }

        return true;
    }

    auto Arc::GetCenter() const -> Eigen::Vector2d
    {
        return _center;
    }

    auto Arc::GetRadius() const -> double
    {
        return _radius;
    }

    auto Arc::GetStartAngle() const -> double
    {
        return _startAngle;
    }

    auto Arc::GetEndAngle() const -> double
    {
        return _endAngle;
    }

    void Arc::SetCenter(const Eigen::Vector2d& center)
    {
        _center = center;
    }

    void Arc::SetRadius(double radius)
    {
        _radius = radius;
    }

    void Arc::SetStartAngle(double angle)
    {
        _startAngle = angle;
    }

    void Arc::SetEndAngle(double angle)
    {
        _endAngle = angle;
    }
}
