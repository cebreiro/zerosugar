#pragma once
#include <any>
#include <coroutine>
#include <variant>
#include <functional>
#include "zerosugar/shared/ai/behavior_tree/node/bt_node_state.h"
#include "zerosugar/shared/ai/behavior_tree/node/bt_node_event.h"
#include "zerosugar/shared/type/not_null_pointer.h"

namespace zerosugar::bt::node
{
    class Result
    {
    public:
        class promise_type
        {
        public:
            auto get_return_object() -> Result;
            auto initial_suspend() -> std::suspend_never;
            auto final_suspend() noexcept -> std::suspend_always;
            void unhandled_exception();
            void return_value(bool success);

            template <bt_event_concept... E>
            auto await_transform(Event<E...>);

            bool IsWaitingFor(const std::type_info& typeInfo) const;

            template <bt_event_concept E>
            void SetEvent(const E& e);
            void SetEvent(const std::any& any);

            auto GetState() const -> State;

        private:
            State _state = State::Success;
            std::any _currentEvent;
            std::function<bool(const std::type_info&)> _eventChecker;
        };

    public:
        Result() = delete;

        explicit(false) Result(bool success);
        explicit(true) Result(std::coroutine_handle<promise_type> handle);

        auto GetState() const -> State;

        auto GetHandle() const -> const std::coroutine_handle<promise_type>&;

    private:
        State _state = State::Success;
        std::coroutine_handle<promise_type> _handle = nullptr;
    };

    template <bt_event_concept ... E>
    auto Result::promise_type::await_transform(Event<E...>)
    {
        _eventChecker = [](const std::type_info& type) -> bool
            {
                return ((type == typeid(E)) || ...);
            };

        class Awaitable
        {
        public:
            explicit Awaitable(PtrNotNull<std::any> event)
                : _event(event)
            {
            }

            bool await_ready() const
            {
                return false;
            }

            void await_suspend(std::coroutine_handle<>) {}

            auto await_resume() const -> std::variant<E...>
            {
                std::variant<E...> event;

                (void)((_event->type() == typeid(E) ?
                            (event = std::move(*std::any_cast<E>(_event)), true) : false
                ) || ...);

                return event;
            }

        private:
            PtrNotNull<std::any> _event;
        };

        _state = State::Running;

        return Awaitable(&_currentEvent);
    }

    template <bt_event_concept E>
    void Result::promise_type::SetEvent(const E& e)
    {
        _currentEvent = e;
    }
}
