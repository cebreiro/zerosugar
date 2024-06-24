#include "game_task_scheduler.h"

#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    bool GameTaskScheduler::Process::Compare::operator()(
        PtrNotNull<const Process> lhs, PtrNotNull<const Process> rhs) const
    {
        if (lhs->Empty() && rhs->Empty())
        {
            return reinterpret_cast<size_t>(lhs) < reinterpret_cast<size_t>(rhs);
        }
        else if (lhs->Empty() && !rhs->Empty())
        {
            return false;
        }
        else if (!lhs->Empty() && rhs->Empty())
        {
            return true;
        }
        else
        {
            const auto lvalue = lhs->Front().GetCreationTimePoint();
            const auto rvalue = rhs->Front().GetCreationTimePoint();

            if (lvalue == rvalue)
            {
                return reinterpret_cast<size_t>(lhs) < reinterpret_cast<size_t>(rhs);
            }

            return lvalue < rvalue;
        }
    }

    void GameTaskScheduler::Process::ResetStarvationCount()
    {
        _starvationCount = 0;
    }

    void GameTaskScheduler::Schedule(std::unique_ptr<GameTaskBase> task)
    {
        Dispatch(*_strand, [self = shared_from_this(), task = std::move(task)]() mutable
            {
                self->ScheduleImpl(std::move(task));
            });
    }

    void GameTaskScheduler::ScheduleImpl(std::unique_ptr<GameTaskBase> task)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        const int64_t processId = task->GetSenderId();

        const auto iter = _processes.find(processId);
        if (iter == _processes.end())
        {
            assert(false);

            return;
        }

        Process& process = iter->second;

        const bool empty = process.Empty();
        process.Push(std::move(task));

        if (empty)
        {
            assert(process.GetState() == Process::State::Empty);

            if (CanSchedule(process))
            {
                ChangeProcessState(process, Process::State::Running);

                StartTask(process);
            }
            else
            {
                ChangeProcessState(process, Process::State::Blocked);
            }
        }
        else
        {
            assert(process.GetState() == Process::State::Blocked);
        }
    }

    auto GameTaskScheduler::FindProcess(int64_t id) const -> const Process*
    {
        const auto iter = _processes.find(id);

        return iter != _processes.end() ? &iter->second : nullptr;
    }

    auto GameTaskScheduler::FindOwnership(int64_t id) const -> const Resource*
    {
        const auto iter = _resources.find(id);

        return iter != _resources.end() ? &iter->second : nullptr;
    }

    bool GameTaskScheduler::CanSchedule(const Process& process) const
    {
        if (process.Empty())
        {
            return false;
        }

        const GameTaskBase& task = process.Front();

        const auto canStart = [process = &process](const Resource* ownership)
            {
                if (!ownership)
                {
                    return true;
                }

                if (ownership->state == ResourceStateType::Acquired)
                {
                    return false;
                }

                if (!ownership->reservedProcesses.empty() && ownership->reservedProcesses.front() != process)
                {
                    return false;
                }

                return true;
            };

        if (const Resource* ownership = FindOwnership(*task.GetMainTargetId());
            canStart(ownership))
        {
            if (std::ranges::all_of(task.GetSubTargetIds(), [canStart, this](int64_t id)
                {
                    return canStart(FindOwnership(id));
                }))
            {
                return true;
            }
        }

        return false;
    }

    void GameTaskScheduler::OnComplete(Process& process)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));
        assert(process.GetState() == Process::State::Running);

        TakeBackResource(process);

        process.Pop();
        ChangeProcessState(process, process.Empty() ? Process::State::Empty : Process::State::Blocked);

        TryScheduleAll();
    }

    void GameTaskScheduler::TryScheduleAll()
    {
        auto& blockedTasks = GetProcessStates(Process::State::Blocked);

        auto iter = blockedTasks.begin();
        for (auto next = std::next(iter); iter != blockedTasks.end(); iter = next++)
        {
            Process* process = iter->first;

            if (process->Empty())
            {
                break;
            }

            if (CanSchedule(*process))
            {
                StartTask(*process);
            }
            else if (process->AddStarvationCount(); process->GetStarvationCount() > 10)
            {
                ReserveResource(*process);
            }
        }
    }

    void GameTaskScheduler::StartTask(Process& process)
    {
        // container 에서 찾아서 target 이 없는 경우 따져봐야함
        // 태스크 버리고 복귀시킬수도?

        process.ResetStarvationCount();

        Post(*_executor, [self = shared_from_this(), process = &process]() mutable
            {
                // task->Start()

                Post(*self->_strand, [self = std::move(self), process]()
                    {
                        self->OnComplete(*process);
                    });
            });
    }

    void GameTaskScheduler::ReserveResource(const Process& process)
    {
        const GameTaskBase& task = process.Front();

        std::optional<int64_t> mainTarget = task.GetMainTargetId();
        assert(mainTarget);

        ReserveResource(*mainTarget, process);

        for (int64_t targetId : task.GetSubTargetIds())
        {
            ReserveResource(targetId, process);
        }
    }

    void GameTaskScheduler::ReserveResource(int64_t id, const Process& process)
    {
        const auto iter = _resources.find(id);
        assert(iter != _resources.end());

        Resource& ownership = iter->second;

        ownership.reservedProcesses.push_back(&process);
    }

    void GameTaskScheduler::GiveResource(Process& process)
    {
        GameTaskBase& task = process.Front();

        std::optional<int64_t> mainTarget = task.GetMainTargetId();
        assert(mainTarget);

        GiveResource(*mainTarget, process);

        for (int64_t targetId : task.GetSubTargetIds())
        {
            GiveResource(targetId, process);
        }
    }

    void GameTaskScheduler::GiveResource(int64_t id, const Process& process)
    {
        const auto iter = _resources.find(id);
        assert(iter != _resources.end());

        Resource& ownership = iter->second;
        assert(ownership.state == ResourceStateType::Released);
        assert(!ownership.acquiredProcess);

        ownership.state = ResourceStateType::Acquired;
        ownership.acquiredProcess = &process;

        if (std::vector<const Process*>& reserved = ownership.reservedProcesses;
            !reserved.empty() && reserved.front() == &process)
        {
            reserved.erase(reserved.begin());
        }
    }

    void GameTaskScheduler::TakeBackResource(Process& process)
    {
        GameTaskBase& task = process.Front();

        std::optional<int64_t> mainTarget = task.GetMainTargetId();
        assert(mainTarget);

        TakeBackResource(*mainTarget);

        for (int64_t id : task.GetSubTargetIds())
        {
            TakeBackResource(id);
        }
    }

    void GameTaskScheduler::TakeBackResource(int64_t id)
    {
        const auto iter = _resources.find(id);
        assert(iter != _resources.end());

        Resource& ownership = iter->second;
        assert(ownership.state == ResourceStateType::Acquired);
        assert(ownership.acquiredProcess && ownership.acquiredProcess == FindProcess(id));

        ownership.state = ResourceStateType::Released;
        ownership.acquiredProcess = nullptr;
    }

    void GameTaskScheduler::ChangeProcessState(Process& process, Process::State newState)
    {
        assert(process.GetState() != newState);

        map_type& olds = GetProcessStates(process.GetState());
        map_type& news = GetProcessStates(newState);

        assert(olds.contains(&process));

        news.insert(olds.extract(&process));
        process.SetState(newState);
    }

    auto GameTaskScheduler::GetProcessStates(Process::State state) -> map_type&
    {
        const int64_t index = static_cast<int32_t>(state);
        assert(index >= 0 && index < std::ssize(_states));

        return _states[index];
    }
}
