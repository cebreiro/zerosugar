#pragma once
#include <cassert>
#include <memory>
#include <ranges>
#include <type_traits>

namespace zerosugar
{
    template <typename T>
    using UniquePtrNotNull = std::unique_ptr<T>;

    template <typename T>
    using SharedPtrNotNull = std::shared_ptr<T>;

    namespace notnull
    {
        static constexpr auto reference = std::views::transform([]<typename T>(const T& ptr)
            -> typename std::pointer_traits<T>::element_type&
            {
                assert(ptr);
                return *ptr;
            });
    }
}
