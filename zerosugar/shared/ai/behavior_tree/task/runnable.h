#pragma once
#include <coroutine>
#include "zerosugar/shared/ai/behavior_tree/task/state.h"

namespace zerosugar::bt
{
    class Runnable
    {
    public:
        struct promise_type
        {
            auto get_return_object() -> Runnable;
            auto initial_suspend() const -> std::suspend_always;
            auto final_suspend() const noexcept -> std::suspend_always;
            void unhandled_exception();
            void return_value(bool value);

            State state = State::None;
        };

        struct Running
        {
            bool await_ready() const;
            void await_suspend(std::coroutine_handle<> handle);
            void await_resume();
        };

        Runnable(const Runnable& other) = delete;
        Runnable& operator=(const Runnable& other) = delete;

        Runnable() = default;
        Runnable(Runnable&& other) noexcept;
        Runnable& operator=(Runnable&& other) noexcept;

        explicit Runnable(std::coroutine_handle<promise_type> handle);
        explicit Runnable(State state);
        ~Runnable();

        bool IsDone() const;
        auto Execute() -> State;

        auto GetState() const -> State;

    private:
        std::coroutine_handle<promise_type> _handle = nullptr;
        State _state = State::None;
    };

    static Runnable::Running running;
}
