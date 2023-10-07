#pragma once
#include <concepts>
#include <string>
#include <format>
#include <functional>

namespace zerosugar
{
    template <std::integral T, typename Tag>
    class ValueType
    {
    public:
        ValueType() = default;
        explicit ValueType(T value);

        explicit operator T() const noexcept;
        auto Unwrap() const noexcept -> T;

        bool operator==(const ValueType& other) const;
        bool operator!=(const ValueType& other) const;
        bool operator<(const ValueType& other) const;

        static auto Default() -> ValueType;

    private:
        T _value = {};
    };

    template <std::integral T, typename Tag>
    ValueType<T, Tag>::ValueType(T value)
        : _value(value)
    {
    }

    template <std::integral T, typename Tag>
    ValueType<T, Tag>::operator T() const noexcept
    {
        return Unwrap();
    }

    template <std::integral T, typename Tag>
    auto ValueType<T, Tag>::Unwrap() const noexcept -> T
    {
        return _value;
    }

    template <std::integral T, typename Tag>
    bool ValueType<T, Tag>::operator==(const ValueType& other) const
    {
        return _value == other._value;
    }

    template <std::integral T, typename Tag>
    bool ValueType<T, Tag>::operator!=(const ValueType& other) const
    {
        return _value != other._value;
    }

    template <std::integral T, typename Tag>
    bool ValueType<T, Tag>::operator<(const ValueType& other) const
    {
        return _value < other._value;
    }

    template <std::integral T, typename Tag>
    auto ValueType<T, Tag>::Default() -> ValueType
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            return ValueType(0);
        }
        else
        {
            return ValueType(-1);
        }
    }
}

namespace std
{
    template <std::integral T, typename Tag>
    struct hash<zerosugar::ValueType<T, Tag>>
    {
        size_t operator()(zerosugar::ValueType<T, Tag> value) const noexcept {
            return std::hash<T>{}(value.Unwrap());
        }
    };

    template <std::integral T, typename Tag>
    struct formatter<zerosugar::ValueType<T, Tag>> : std::formatter<std::string>
    {
        auto format(zerosugar::ValueType<T, Tag> value, std::format_context& context) const
            -> std::format_context::iterator
        {
            return std::formatter<std::string>::format(std::format("{}", value.Unwrap()), context);
        }
    };
}
