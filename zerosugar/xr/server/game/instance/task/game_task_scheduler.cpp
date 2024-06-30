#include "game_task_scheduler.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    bool GameTaskScheduler::Process::Compare::operator()(
        PtrNotNull<const Process> lhs, PtrNotNull<const Process> rhs) const
    {
        const auto lvalue = lhs->HasTask() ? lhs->GetTask().GetCreationTimePoint().time_since_epoch().count() : std::numeric_limits<int64_t>::max();
        const auto rvalue = rhs->HasTask() ? rhs->GetTask().GetCreationTimePoint().time_since_epoch().count() : std::numeric_limits<int64_t>::max();

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

    GameTaskScheduler::~GameTaskScheduler()
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

    void GameTaskScheduler::AddController(game_controller_id_type id)
    {
        Dispatch(_gameInstance.GetStrand(), [this, id = id.Unwrap()]()
            {
                if (_processTaskQueues.try_emplace(id, std::queue<UniquePtrNotNull<GameTask>>()).second)
                {
                    Process& process = CreateProcess(id);

                    process.SetTaskQueueId(id);
                }
                else
                {
                    assert(false);
                }
            });
    }

    void GameTaskScheduler::RemoveController(game_controller_id_type id)
    {
        Dispatch(_gameInstance.GetStrand(), [this, id = id.Unwrap()]()
            {
                [[maybe_unused]]
                const size_t erased = _processTaskQueues.erase(id);
                assert(erased);
            });
    }

    void GameTaskScheduler::AddEntity(game_entity_id_type id)
    {
        Dispatch(_gameInstance.GetStrand(), [this, id = id.Unwrap()]()
            {
                [[maybe_unused]]
                const bool inserted = _resources.try_emplace(id, Resource{}).second;
                assert(inserted);
            });
    }

    void GameTaskScheduler::RemoveEntity(game_entity_id_type id)
    {
        Dispatch(_gameInstance.GetStrand(), [this, id = id.Unwrap()]()
            {
                [[maybe_unused]]
                const size_t erased = _resources.erase(id);
                assert(erased);
            });
    }

    void GameTaskScheduler::Schedule(std::unique_ptr<GameTask> task, std::optional<game_controller_id_type> controllerId)
    {
        _scheduledTaskCount.fetch_add(1);

        Dispatch(_gameInstance.GetStrand(), [this, task = std::move(task), controllerId = controllerId]() mutable
            {
                ScheduleImpl(std::move(task), controllerId ? controllerId->Unwrap() : std::optional<int64_t>{});
            });
    }

    auto GameTaskScheduler::GetScheduledTaskCount() const -> int64_t
    {
        return _scheduledTaskCount.load();
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
            process = &CreateProcess(--_nextTempProcessId);
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
            assert(!process->HasTask());

            ChangeState(*process, Process::State::Ready, std::move(task));

            if (CanStart(*process))
            {
                ChangeState(*process, Process::State::Running);

                Start(*process);
            }
        }
        break;
        default:
            assert(false);
        }
    }

    auto GameTaskScheduler::FindProcess(int64_t id) const -> const Process*
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        const auto iter = _processes.find(id);

        return iter != _processes.end() ? &iter->second : nullptr;
    }

    auto GameTaskScheduler::FindResource(int64_t id) const -> const Resource*
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        const auto iter = _resources.find(id);

        return iter != _resources.end() ? &iter->second : nullptr;
    }

    bool GameTaskScheduler::CanStart(const Process& process) const
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

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
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        auto& readyProcesses = GetProcessesBy(Process::State::Ready);

        // iter is invalidate so can't use ++iter
        auto iter = readyProcesses.begin();
        while (iter != readyProcesses.end())
        {
            auto next = std::next(iter);

            Process* process = *iter;
            assert(process->HasTask() && process->GetState() == Process::State::Ready);

            if (CanStart(*process))
            {
                ChangeState(*process, Process::State::Running);

                Start(*process);
            }
            else if (process->AddStarvationCount(); process->GetStarvationCount() > 10)
            {
                ReserveResource(*process);
            }

            if (next != readyProcesses.end())
            {
                iter = next++;
            }
            else
            {
                break;
            }
        }
    }

    void GameTaskScheduler::Start(Process& process)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));
        assert(CanStart(process));

        AllocateResource(process);

        Post(_gameInstance.GetExecutor(), [this, process = &process]() mutable
            {
                process->GetTask().Start(_gameInstance.GetParallelContext());

                Dispatch(_gameInstance.GetStrand(), [this, process]()
                    {
                        process->GetTask().Complete(_gameInstance.GetSerialContext());
                        
                        OnComplete(*process);
                    });
            });
    }

    void GameTaskScheduler::OnComplete(Process& process)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));
        assert(process.GetState() == Process::State::Running);
        assert(process.HasTask());

        _scheduledTaskCount.fetch_sub(1);

        DeallocateResource(process);

        std::queue<UniquePtrNotNull<GameTask>>* taskQueue = [this, &process]() -> std::queue<UniquePtrNotNull<GameTask>>*
            {
                const std::optional<int64_t> taskQueueId = process.GetTaskQueueId();
                if (!taskQueueId.has_value())
                {
                    return nullptr;
                }

                if (const auto iter = _processTaskQueues.find(*taskQueueId); iter != _processTaskQueues.end())
                {
                    return &iter->second;
                }

                return nullptr;
            }();

        if (taskQueue)
        {
            if (taskQueue->empty())
            {
                constexpr auto newState = Process::State::Waiting;

                ChangeState(process, newState, std::unique_ptr<GameTask>());
            }
            else
            {
                auto newTask = std::move(taskQueue->front());
                taskQueue->pop();

                constexpr auto newState = Process::State::Ready;

                ChangeState(process, newState, std::move(newTask));
            }
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

                (void)resource.reserved.emplace(&process);
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
                if (iterResource == _resources.end())
                {
                    return;
                }

                Resource& resource = iterResource->second;
                assert(resource.state == Resource::StateType::Free);
                assert(!resource.acquired);

                resource.state = Resource::StateType::Assigned;
                resource.acquired = &process;

                if (auto& reserved = resource.reserved; !reserved.empty())
                {
                    (void)reserved.erase(&process);
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

        const auto deallocate = [this, &process](int64_t id)
            {
                const auto iter = _resources.find(id);
                if (iter == _resources.end())
                {
                    return;
                }

                Resource& resource = iter->second;
                assert(resource.state == Resource::StateType::Assigned);
                assert(resource.acquired && resource.acquired == &process);

                resource.state = Resource::StateType::Free;
                resource.acquired = nullptr;
            };

        for (int64_t id : process.GetTask().GetTargetIds())
        {
            deallocate(id);
        }
    }

    void GameTaskScheduler::ChangeState(Process& process, Process::State newState, std::optional<std::unique_ptr<GameTask>> newTask)
    {
        assert(process.GetState() != newState);

        set_type& olds = GetProcessesBy(process.GetState());
        set_type& news = GetProcessesBy(newState);

        assert(olds.contains(&process));

        auto handle = olds.extract(&process);
        assert(handle);

        // To maintain compare order and ensure it can be found, 'process.SetTask' must be called in place after extract()
        if (newTask)
        {
            process.SetTask(std::move(*newTask));
        }

        [[maybe_unused]]
        bool inserted = news.insert(std::move(handle)).inserted;
        assert(inserted);

        process.SetState(newState);
        process.SetStarvationCount(0);
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

        process->SetId(id);
        process->SetStarvationCount(0);
        process->SetState(Process::State::Waiting);

        return *process;
    }

    void GameTaskScheduler::ExitProcess(Process& process)
    {
        assert(process.GetState() == Process::State::Running);

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

        if (std::optional<int64_t> queueId = process.GetTaskQueueId(); queueId.has_value())
        {
            [[maybe_unused]]
            const size_t erased = _processTaskQueues.erase(*queueId);
            assert(erased > 0);
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
