#pragma once
#include <functional>
#include <concepts>
#include <type_traits>
#include <boost/system/error_code.hpp>

namespace zerosugar::execution
{
    class IExecutor
    {
    public:
        virtual ~IExecutor() = default;

        virtual void Run() = 0;
        virtual void Stop() = 0;
        virtual void Join(std::vector<boost::system::error_code>* errors) = 0;

        virtual void Post(const std::function<void()>& function) = 0;
        virtual void Post(std::move_only_function<void()> function) = 0;
    };

    template <typename T, typename... Args> requires std::invocable<T, Args...>
    void Post(IExecutor& executor, T&& function, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            using function_type = std::conditional_t<
                std::is_move_constructible_v<T>,
                std::move_only_function<void()>, std::function<void()>>;

            executor.Post(function_type(std::forward<T>(function)));
        }
        else
        {
            using function_type = std::conditional_t<
                std::is_move_constructible_v<T> && std::conjunction_v<std::is_move_constructible<Args>...>,
                std::move_only_function<void()>, std::function<void()>>;

            executor.Post(function_type(
                [fn = std::forward<T>(function), ...args = std::forward<Args>(args)]() mutable
                {
                    std::invoke(fn, std::forward<Args>(args)...);
                }));
        }
    }
}
