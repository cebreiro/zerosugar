#include "navi_vector.h"

namespace zerosugar::xr::navi
{
    constexpr const float scale = 100.f;

    Vector::Vector(float x, float y, float z)
        : _data({ x, y, z})
    {
    }

    Vector::Vector(const Eigen::Vector3d& vector)
        : _data({ static_cast<float>(-vector.x() / scale), static_cast<float>(vector.z()) / scale, static_cast<float>(-vector.y()) / scale })
    {
    }

    Vector::Vector(const FVector& vector)
        : _data({ -vector.GetX() / scale, vector.GetZ() / scale, -vector.GetY() / scale })
    {
    }

    Vector::Vector(const std::array<float, 3>& array)
        : _data(array)
    {
    }

    auto Vector::GetX() const -> float
    {
        return _data[0];
    }

    auto Vector::GetY() const -> float
    {
        return _data[1];
    }

    auto Vector::GetZ() const -> float
    {
        return _data[2];
    }

    void Vector::SetX(float x)
    {
        _data[0] = x;
    }

    void Vector::SetY(float y)
    {
        _data[1] = y;
    }

    void Vector::SetZ(float z)
    {
        _data[2] = z;
    }

    auto Vector::GetData() -> float*
    {
        return _data.data();
    }

    auto Vector::GetData() const -> const float*
    {
        return _data.data();
    }

    FVector::FVector(const Eigen::Vector3d& vector)
        : _data({ static_cast<float>(vector.x()), static_cast<float>(vector.y()), static_cast<float>(vector.z()) })
    {
    }

    FVector::FVector(float x, float y, float z)
        : _data({ x, y, z })
    {
    }

    FVector::FVector(const Vector& vector)
        : _data({ -vector.GetX() * scale, -vector.GetZ() * scale, vector.GetY() * scale })
    {
    }

    FVector::FVector(const std::array<float, 3>& array)
        : _data(array)
    {
    }

    auto FVector::GetX() const -> float
    {
        return _data[0];
    }

    auto FVector::GetY() const -> float
    {
        return _data[1];
    }

    auto FVector::GetZ() const -> float
    {
        return _data[2];
    }

    Extents::Extents(float x, float y, float z)
        : _data({ x, y, z })
    {
    }

    Extents::Extents(const FVector& vector)
        : _data({ vector.GetX(), vector.GetZ(), vector.GetY() })
    {
    }

    Extents::Extents(const std::array<float, 3>& array)
        : _data(array)
    {
    }

    auto Extents::GetX() const -> float
    {
        return _data[0];
    }

    auto Extents::GetY() const -> float
    {
        return _data[1];
    }

    auto Extents::GetZ() const -> float
    {
        return _data[2];
    }

    void Extents::SetX(float x)
    {
        _data[0] = x;
    }

    void Extents::SetY(float y)
    {
        _data[1] = y;
    }

    void Extents::SetZ(float z)
    {
        _data[2] = z;
    }

    auto Extents::GetData() -> float*
    {
        return _data.data();
    }

    auto Extents::GetData() const -> const float*
    {
        return _data.data();
    }

    Scalar::Scalar(float value)
        : _value(value / scale)
    {
    }

    auto Scalar::Get() const -> float
    {
        return _value;
    }
}
