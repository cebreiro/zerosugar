#pragma once
#include <Eigen/Dense>

namespace zerosugar::collision
{
    class Arc
    {
    public:
        Arc() = default;
        Arc(Eigen::Vector2d center, double radius, double startAngle, double endAngle);

        bool Contains(double angle) const;

        auto GetCenter() const -> Eigen::Vector2d;
        auto GetRadius() const -> double;
        auto GetStartAngle() const -> double;
        auto GetEndAngle() const -> double;

        void SetCenter(const Eigen::Vector2d& center);
        void SetRadius(double radius);
        void SetStartAngle(double angle);
        void SetEndAngle(double angle);

    private:
        Eigen::Vector2d _center = {};
        double _radius = 0.0;
        double _startAngle = 0.0;
        double _endAngle = 0.0;
    };
}
