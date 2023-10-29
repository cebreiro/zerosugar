#include "runnable.h"

#include <cassert>
#include <utility>

namespace zerosugar::bt
{
    auto Runnable::promise_type::get_return_object() -> Runnable
    {
        return Runnable(std::coroutine_handle<promise_type>::from_promise(*this));
    }

    auto Runnable::promise_type::initial_suspend() const -> std::suspend_always
    {
        return std::suspend_always{};
    }

    auto Runnable::promise_type::final_suspend() const noexcept -> std::suspend_always
    {
        return std::suspend_always{};
    }

    void Runnable::promise_type::unhandled_exception()
    {
        assert(false);
    }

    void Runnable::promise_type::return_value(bool value)
    {
        this->state = value ? State::Success : State::Failure;
    }

    bool Runnable::Running::await_ready() const
    {
        return false;
    }

    void Runnable::Running::await_suspend(std::coroutine_handle<> handle)
    {
        promise_type& promise = std::coroutine_handle<promise_type>::from_address(
            handle.address()).promise();
        promise.state = State::Running;
    }

    void Runnable::Running::await_resume()
    {
    }

    Runnable::Runnable(Runnable&& other) noexcept
        : _handle(std::exchange(other._handle, nullptr))
        , _state(std::exchange(other._state, State::None))
    {
    }

    Runnable& Runnable::operator=(Runnable&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        this->~Runnable();

        _handle = std::exchange(other._handle, nullptr);
        _state = std::exchange(other._state, State::None);

        return *this;
    }

    Runnable::Runnable(std::coroutine_handle<promise_type> handle)
        : _handle(handle)
    {
    }

    Runnable::Runnable(State state)
        : _state(state)
    {
    }

    Runnable::~Runnable()
    {
    }

    bool Runnable::IsDone() const
    {
        if (_handle == nullptr)
        {
            assert(_state != State::None && _state != State::Running);

            return _state == State::Success || _state == State::Failure;
        }

        return _handle.done();
    }

    auto Runnable::Execute() -> State
    {
        assert(_handle);
        assert(!_handle.done());

        _handle.resume();
        _state = _handle.promise().state;

        return _state;
    }

    auto Runnable::GetState() const -> State
    {
        return _state;
    }
}
