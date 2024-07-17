#pragma once
#include <array>

namespace zerosugar::xr::navi
{
    class FVector;

    class Vector
    {
    public:
        Vector() = default;

        Vector(float x, float y, float z);

        explicit (true) Vector(const Eigen::Vector3d& vector);
        explicit(false) Vector(const FVector& vector);
        explicit(false) Vector(const std::array<float, 3>& array);

        auto GetX() const -> float;
        auto GetY() const -> float;
        auto GetZ() const -> float;

        void SetX(float x);
        void SetY(float y);
        void SetZ(float z);

        auto GetData() -> float*;
        auto GetData() const -> const float*;

    private:
        std::array<float, 3> _data = {};
    };

    // unreal coordinate system indicator
    class FVector
    {
    public:
        FVector() = default;

        FVector(float x, float y, float z);

        explicit (true) FVector(const Eigen::Vector3d& vector);
        explicit(false) FVector(const Vector& vector);
        explicit(false) FVector(const std::array<float, 3>& array);

        auto GetX() const -> float;
        auto GetY() const -> float;
        auto GetZ() const -> float;

    private:
        std::array<float, 3> _data = {};
    };

    class Extents
    {
    public:
        Extents(float x, float y, float z);
        explicit(false) Extents(const FVector& vector);
        explicit(false) Extents(const std::array<float, 3>& array);

        auto GetX() const -> float;
        auto GetY() const -> float;
        auto GetZ() const -> float;

        void SetX(float x);
        void SetY(float y);
        void SetZ(float z);

        auto GetData() -> float*;
        auto GetData() const -> const float*;

    private:
        std::array<float, 3> _data = {};
    };

    class Scalar
    {
    public:
        Scalar() = default;
        explicit Scalar(float value);
        explicit Scalar(double value);

        auto Get() const -> float;

    private:
        float _value = 0.f;
    };
}
