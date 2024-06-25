#include "game_task_scheduler.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    bool GameTaskScheduler::Process::Compare::operator()(
        PtrNotNull<const Process> lhs, PtrNotNull<const Process> rhs) const
    {
        const auto lvalue = lhs->HasTask() ? lhs->GetTask().GetCreationTimePoint() : std::numeric_limits<int64_t>::max();
        const auto rvalue = rhs->HasTask() ? rhs->GetTask().GetCreationTimePoint() : std::numeric_limits<int64_t>::max();

        if (lvalue == rvalue)
        {
            return reinterpret_cast<size_t>(lhs) < reinterpret_cast<size_t>(rhs);
        }

        return lvalue < rvalue;
    }

    GameTaskScheduler::Process::Process(int64_t id)
        : _id(id)
    {
    }

    bool GameTaskScheduler::Process::HasTask() const
    {
        return _task.operator bool();
    }

    void GameTaskScheduler::Process::AddStarvationCount()
    {
        ++_starvationCount;
    }

    auto GameTaskScheduler::Process::GetId() const -> int64_t
    {
        return _id;
    }

    auto GameTaskScheduler::Process::GetTask() const -> GameTask&
    {
        assert(_task);

        return *_task;
    }

    auto GameTaskScheduler::Process::GetTaskQueueId() const -> std::optional<int64_t>
    {
        return _taskQueueId;
    }

    auto GameTaskScheduler::Process::GetState() const -> State
    {
        return _state;
    }

    auto GameTaskScheduler::Process::GetStarvationCount() const -> int64_t
    {
        return _starvationCount;
    }

    void GameTaskScheduler::Process::SetId(int64_t id)
    {
        _id = id;
    }

    void GameTaskScheduler::Process::SetTask(std::unique_ptr<GameTask> task)
    {
        _task = std::move(task);
    }

    void GameTaskScheduler::Process::SetTaskQueueId(std::optional<int64_t> id)
    {
        _taskQueueId = id;
    }

    void GameTaskScheduler::Process::SetState(State state)
    {
        _state = state;
    }

    void GameTaskScheduler::Process::SetStarvationCount(int64_t value)
    {
        _starvationCount = value;
    }

    GameTaskScheduler::GameTaskScheduler(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    void GameTaskScheduler::Shutdown()
    {
        _shutdown = true;
    }

    auto GameTaskScheduler::Join() -> Future<void>
    {
        if (!ExecutionContext::Contains(_gameInstance.GetStrand()))
        {
            co_await _gameInstance.GetStrand();
        }

        set_type& processes = GetProcessesBy(Process::State::Running);
        if (processes.empty())
        {
            co_return;
        }

        _shutdownJoinPromise.emplace();

        Future<void> future = _shutdownJoinPromise->GetFuture();
        co_await future;

        co_return;
    }

    auto GameTaskScheduler::AddProcess() -> Future<int64_t>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::Contains(_gameInstance.GetStrand()))
        {
            co_await _gameInstance.GetStrand();
        }

        const int64_t taskQueueId = ++_nextTaskQueueId;

        if (_processTaskQueues.try_emplace(taskQueueId, std::vector<GameTask>()).second)
        {
            Process& process = CreateProcess(taskQueueId);

            process.SetTaskQueueId(taskQueueId);
        }
        else
        {
            assert(false);
        }

        co_return taskQueueId;
    }

    auto GameTaskScheduler::RemoveProcess(int64_t id) -> Future<bool>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::Contains(_gameInstance.GetStrand()))
        {
            co_await _gameInstance.GetStrand();
        }

        co_return _processTaskQueues.erase(id);
    }

    auto GameTaskScheduler::AddResource(int64_t id) -> Future<bool>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::Contains(_gameInstance.GetStrand()))
        {
            co_await _gameInstance.GetStrand();
        }

        co_return _resources.try_emplace(id, Resource{}).second;
    }

    auto GameTaskScheduler::RemoveResource(int64_t id) -> Future<bool>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::Contains(_gameInstance.GetStrand()))
        {
            co_await _gameInstance.GetStrand();
        }

        co_return _resources.erase(id);
    }

    void GameTaskScheduler::Schedule(std::unique_ptr<GameTask> task, std::optional<int64_t> processId)
    {
        Dispatch(_gameInstance.GetStrand(), [self = shared_from_this(), task = std::move(task), processId]() mutable
            {
                self->ScheduleImpl(std::move(task), processId);
            });
    }

    void GameTaskScheduler::ScheduleImpl(std::unique_ptr<GameTask> task, std::optional<int64_t> processId)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (_shutdown)
        {
            return;
        }

        Process* process = nullptr;

        if (processId.has_value())
        {
            const auto iter = _processes.find(*processId);
            if (iter != _processes.end())
            {
                process = &iter->second;
            }
            else
            {
                assert(false);
            }
        }

        if (!process)
        {
            process = &CreateProcess(++_nextProcessId);
        }

        switch (process->GetState())
        {

        case Process::State::Running:
        case Process::State::Ready:
        {
            const auto iter = _processTaskQueues.find(process->GetId());
            assert(iter != _processTaskQueues.end());

            iter->second.push(std::move(task));
        }
        break;
        case Process::State::Waiting:
        {
            process->SetTask(std::move(task));

            if (CanStart(*process))
            {
                ChangeState(*process, Process::State::Running);

                Start(*process);
            }
            else
            {
                ChangeState(*process, Process::State::Ready);
            }
        }
        break;
        default:
            assert(false);
        }
    }

    auto GameTaskScheduler::FindProcess(int64_t id) const -> const Process*
    {
        const auto iter = _processes.find(id);

        return iter != _processes.end() ? &iter->second : nullptr;
    }

    auto GameTaskScheduler::FindResource(int64_t id) const -> const Resource*
    {
        const auto iter = _resources.find(id);

        return iter != _resources.end() ? &iter->second : nullptr;
    }

    bool GameTaskScheduler::CanStart(const Process& process) const
    {
        if (!process.HasTask())
        {
            assert(false);

            return false;
        }

        const GameTask& task = process.GetTask();

        const auto canAcquire = [process = &process](const Resource* resource)
            {
                if (!resource)
                {
                    return true;
                }

                if (resource->state == Resource::StateType::Assigned)
                {
                    return false;
                }

                if (!resource->reserved.empty() && !resource->reserved.contains(process))
                {
                    return false;
                }

                return true;
            };

        return std::ranges::all_of(task.GetTargetIds(), [canAcquire, this](int64_t id)
            {
                return canAcquire(FindResource(id));
            });
    }

    void GameTaskScheduler::TryStartAll()
    {
        auto& readyProcesses = GetProcessesBy(Process::State::Ready);

        // iter is invalidate so can't use ++iter
        auto iter = readyProcesses.begin();
        for (auto next = std::next(iter); iter != readyProcesses.end(); iter = next++)
        {
            Process* process = *iter;
            assert(process->HasTask());

            if (CanStart(*process))
            {
                ChangeState(*process, Process::State::Running);

                Start(*process);
            }
            else if (process->AddStarvationCount(); process->GetStarvationCount() > 10)
            {
                ReserveResource(*process);
            }
        }
    }

    void GameTaskScheduler::Start(Process& process)
    {
        Post(_gameInstance.GetExecutor(), [self = shared_from_this(), process = &process]() mutable
            {
                process->GetTask().Start(self->_gameInstance);

                Strand& strand = self->_gameInstance.GetStrand();

                Dispatch(strand, [self = std::move(self), process]()
                    {
                        process->GetTask().Complete(self->_gameInstance);
                        
                        self->OnComplete(*process);
                    });
            });
    }

    void GameTaskScheduler::OnComplete(Process& process)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));
        assert(process.GetState() == Process::State::Running);
        assert(process.HasTask());

        DeallocateResource(process);

        const std::optional<int64_t> taskQueueId = process.GetTaskQueueId();

        std::optional<Process::State> newState = std::nullopt;

        do
        {
            if (!taskQueueId.has_value())
            {
                break;
            }

            const int64_t queueId = *taskQueueId;

            const auto iter = _processTaskQueues.find(queueId);
            if (iter == _processTaskQueues.end())
            {
                break;
            }

            if (auto& queue = iter->second; queue.empty())
            {
                newState = Process::State::Waiting;
            }
            else
            {
                std::unique_ptr<GameTask> task = std::move(queue.front());
                queue.pop();

                process.SetTask(std::move(task));
                process.SetStarvationCount(0);

                newState = Process::State::Ready;
            }

            
        } while (false);

        if (newState)
        {
            ChangeState(process, *newState);
        }
        else
        {
            ExitProcess(process);
        }

        if (_shutdown)
        {
            set_type& runningProcesses = GetProcessesBy(Process::State::Running);
            if (runningProcesses.empty() && _shutdownJoinPromise)
            {
                _shutdownJoinPromise->Set();
            }

            return;
        }

        TryStartAll();
    }

    void GameTaskScheduler::ReserveResource(const Process& process)
    {
        assert(process.HasTask());

        const auto reserve = [this](int64_t id, const Process& process)
            {
                const auto iter = _resources.find(id);
                assert(iter != _resources.end());

                Resource& resource = iter->second;

                [[maybe_unused]]
                const bool inserted = resource.reserved.emplace(&process).second;
                assert(inserted);
            };

        for (int64_t targetId : process.GetTask().GetTargetIds())
        {
            reserve(targetId, process);
        }
    }

    void GameTaskScheduler::AllocateResource(const Process& process)
    {
        assert(process.HasTask());

        const auto allocate = [this](int64_t id, const Process& process)
            {
                const auto iterResource = _resources.find(id);
                assert(iterResource != _resources.end());

                Resource& resource = iterResource->second;
                assert(resource.state == Resource::StateType::Free);
                assert(!resource.acquired);

                resource.state = Resource::StateType::Assigned;
                resource.acquired = &process;

                if (auto& reserved = resource.reserved; !reserved.empty())
                {
                    if (const auto iter = reserved.find(&process); iter != reserved.end())
                    {
                        reserved.erase(iter);
                    }
                }
            };

        for (int64_t targetId : process.GetTask().GetTargetIds())
        {
            allocate(targetId, process);
        }
    }

    void GameTaskScheduler::DeallocateResource(const Process& process)
    {
        assert(process.HasTask());

        const GameTask& task = process.GetTask();

        const auto deallocate = [this, &process](int64_t id)
            {
                const auto iter = _resources.find(id);
                if (iter == _resources.end())
                {
                    return;
                }

                Resource& ownership = iter->second;
                assert(ownership.state == Resource::StateType::Assigned);
                assert(ownership.acquired && ownership.acquired == &process);

                ownership.state = Resource::StateType::Free;
                ownership.acquired = nullptr;
            };

        for (int64_t id : process.GetTask().GetTargetIds())
        {
            deallocate(id);
        }
    }

    void GameTaskScheduler::ChangeState(Process& process, Process::State newState)
    {
        assert(process.GetState() != newState);

        set_type& olds = GetProcessesBy(process.GetState());
        set_type& news = GetProcessesBy(newState);

        assert(olds.contains(&process));

        auto handle = olds.extract(&process);
        assert(handle);

        [[maybe_unused]]
        bool inserted = news.insert(std::move(handle)).inserted;
        assert(inserted);

        process.SetState(newState);
    }

    auto GameTaskScheduler::CreateProcess(int64_t id) -> Process&
    {
        Process* process = nullptr;

        if (_processesRecycleBuffer.empty())
        {
            [[maybe_unused]]
            const auto& [iter, inserted] = _processes.emplace(id, Process());
            assert(inserted);

            process = &iter->second;
        }
        else
        {
            auto handle = _processesRecycleBuffer.extract(_processesRecycleBuffer.begin());
            assert(handle);

            handle.key() = id;
            handle.mapped() = Process(id);

            [[maybe_unused]]
            const auto& [iter, inserted, _] = _processes.insert(std::move(handle));
            assert(inserted);

            process = &iter->second;
        }


        set_type& processes = GetProcessesBy(Process::State::Waiting);

        if (_statesRecycleBuffer.empty())
        {
            processes.insert(process);
        }
        else
        {
            auto handle = _statesRecycleBuffer.extract(_statesRecycleBuffer.begin());
            assert(handle);

            handle.value() = process;

            [[maybe_unused]]
            const bool inserted = processes.insert(std::move(handle)).inserted;
            assert(inserted);
        }

        return *process;
    }

    void GameTaskScheduler::ExitProcess(Process& process)
    {
        const int64_t id = ++_nextRecycleProcessId;

        set_type& states = GetProcessesBy(process.GetState());
        if (auto handle = states.extract(&process); handle)
        {
            handle.value() = reinterpret_cast<Process*>(id);

            [[maybe_unused]]
            const bool inserted = _statesRecycleBuffer.insert(std::move(handle)).inserted;
            assert(inserted);
        }
        else
        {
            assert(false);
        }

        if (auto handle = _processes.extract(process.GetId()); handle)
        {
            handle.key() = id;
            handle.mapped().SetId(id);

            [[maybe_unused]]
            const bool inserted = _processesRecycleBuffer.insert(std::move(handle)).inserted;
            assert(inserted);
        }
        else
        {
            assert(false);
        }
    }

    auto GameTaskScheduler::GetProcessesBy(Process::State state) -> set_type&
    {
        const int64_t index = static_cast<int32_t>(state);
        assert(index >= 0 && index < std::ssize(_states));

        return _states[index];
    }
}
