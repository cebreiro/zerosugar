#pragma once
#include <Eigen/Geometry>

namespace zerosugar::collision
{
    class OBB;
}

namespace zerosugar::xr
{
    class GameSpatialMBR
    {
    public:
        GameSpatialMBR() = default;
        GameSpatialMBR(const Eigen::Vector2d& min, const Eigen::Vector2d& max);
        GameSpatialMBR(const Eigen::Vector2d& pos, double radius);

        void Extend(const GameSpatialMBR& other);

        bool Intersect(const GameSpatialMBR& other) const;

        auto GetMin() const-> Eigen::Vector2d;
        auto GetMax() const-> Eigen::Vector2d;

        static auto CreateFrom(const collision::OBB& obb) -> GameSpatialMBR;

    private:
        Eigen::AlignedBox2d _box = {};
    };
}
