#pragma once

namespace zerosugar::xr::bot
{
    class IVisualObject
    {
    public:
        virtual ~IVisualObject() = default;

        virtual auto GetPosition() const -> const Eigen::Vector3d& = 0;
        virtual auto GetRotation() const -> const Eigen::Vector3d & = 0;
    };
}
