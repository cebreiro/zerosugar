#include "intersect.h"

#include "zerosugar/shared/collision/shape/arc.h"
#include "zerosugar/shared/collision/shape/circle.h"
#include "zerosugar/shared/collision/shape/obb.h"

namespace zerosugar::collision
{
    auto GetAngle(const Eigen::Vector2d& v1, const Eigen::Vector2d& v2)
    {
        const double dot = v1.dot(v2);
        const double det = v1.x() * v2.y() - v1.y() * v2.x();

        return std::atan2(det, dot);
    }

    bool Intersect(const Arc& arc, const Circle& circle)
    {
        Eigen::Vector2d diff = circle.GetCenter() - arc.GetCenter();
        const double distance = diff.norm();

        if (distance > arc.GetRadius() + circle.GetRadius())
        {
            return false;
        }

        if (distance <= circle.GetRadius())
        {
            return true;
        }

        const double angle = GetAngle(Eigen::Vector2d(1, 0), diff.normalized());

        return arc.Contains(angle);
    }

    bool Intersect(const OBB& obb, const Circle& circle)
    {
        const Eigen::Vector2d diff = circle.GetCenter() - obb.GetCenter();
        const Eigen::Vector2d localCircleCenter = obb.GetRotation().transpose() * diff;

        const double halfSizeX = obb.GetHalfSize().x();
        const double halfSizeY = obb.GetHalfSize().y();

        Eigen::Vector2d clamped;
        clamped.x() = std::max(-halfSizeX, std::min(localCircleCenter.x(), halfSizeX));
        clamped.y() = std::max(-halfSizeY, std::min(localCircleCenter.y(), halfSizeY));

        return (localCircleCenter - clamped).squaredNorm() <= (circle.GetRadius() * circle.GetRadius());
    }

    bool Intersect(const Circle& lhs, const Circle& rhs)
    {
        const double distance = (lhs.GetCenter() - rhs.GetCenter()).norm();

        return distance <= (lhs.GetRadius() + rhs.GetRadius());
    }

    bool Intersect(const Circle& circle, const Arc& arc)
    {
        return Intersect(arc, circle);
    }

    bool Intersect(const Circle& circle, const OBB& obb)
    {
        return Intersect(obb, circle);
    }
}
