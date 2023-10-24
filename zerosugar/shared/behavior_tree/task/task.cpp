#include "task.h"

#include <cassert>

namespace zerosugar::bt
{
    auto Task::Execute() -> State
    {
        if (!_runnable.has_value())
        {
            _runnable = this->Run();
            _state = _runnable->GetState();
        }

        if (_runnable->IsDone())
        {
            _state = _runnable->GetState();
        }
        else
        {
            _state = _runnable->Execute();
        }

        assert(_state != State::None);

        if (_state != State::Running)
        {
            _runnable.reset();
        }

        return _state;
    }

    void Task::Reset()
    {
        _state = State::None;
        _runnable.reset();
    }

    auto Task::GetState() const -> State
    {
        return _state;
    }
}
