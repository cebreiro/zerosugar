#include "navi_vector.h"

namespace zerosugar::xr::navi
{
    Vector::Vector(float x, float y, float z)
        : _data({ x, y, z})
    {
    }

    Vector::Vector(const FVector& vector)
        : _data({ -vector.GetX(), vector.GetZ(), -vector.GetY() })
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

    FVector::FVector(float x, float y, float z)
        : _data({ x, y, z })
    {
    }

    FVector::FVector(const Vector& vector)
        : _data({ -vector.GetX(), vector.GetZ(), -vector.GetY() })
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
}
