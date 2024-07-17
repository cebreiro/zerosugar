#pragma once

namespace zerosugar::xr::navi::vis
{
    enum class DrawColor
    {
        Red,
        Blue,
        Green,
        Brown,
        Cyan,
        Yellow,
        LightBlue,
        White,
    };

    auto ToInt(DrawColor color) -> uint32_t;
    auto ToArray(DrawColor color) -> std::array<uint8_t, 4>;

    struct Agent
    {
        Eigen::Vector3d position;
        float yaw = 0.f;
        double radius = 0.0;
        DrawColor drawColor = DrawColor::Green;

        struct Movement
        {
            Eigen::Vector3d destPosition;
            std::chrono::system_clock::time_point startTimePoint;
            double duration = 0.0;
            DrawColor drawColor = DrawColor::Yellow;
        };
        std::optional<Movement> movement = std::nullopt;
    };

    struct Cylinder
    {
        Eigen::Vector3d min;
        Eigen::Vector3d max;
    };

    struct OBB
    {
        Eigen::Vector3d center;
        Eigen::Vector3d halfSize;
        Eigen::Matrix3d rotation;
    };

    struct Circle
    {
        Eigen::Vector3d center;
        double radius = 0.0;
    };

    struct Arrow
    {
        Eigen::Vector3d startPos;
        Eigen::Vector3d endPos;
    };

    struct Lines
    {
        std::vector<Eigen::Vector3d> positions;
    };

    struct Object
    {
        using shape_type = std::variant<Cylinder, OBB, Circle, Arrow, Lines>;

        shape_type shape;
        DrawColor drawColor = DrawColor::Green;
    };
}
