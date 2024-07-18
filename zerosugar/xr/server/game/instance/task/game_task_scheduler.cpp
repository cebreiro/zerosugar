#include "game_task_scheduler.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"

namespace zerosugar::xr
{
    bool GameTaskScheduler::Process::Compare::operator()(
        PtrNotNull<const Process> lhs, PtrNotNull<const Process> rhs) const
    {
        const int64_t lPriority = lhs->GetPriority();
        const int64_t rPriority = rhs->GetPriority();

        if (lPriority == rPriority)
        {
            return lhs < rhs;
        }

        return lPriority < rPriority;
    }

    GameTaskScheduler::Process::Process(int64_t id)
        : _id(id)
    {
    }

    bool GameTaskScheduler::Process::HasTask() const
    {
        return _task.operator bool();
    }

    bool GameTaskScheduler::Process::IsTerminateReserved() const
    {
        return _terminateReserved;
    }

    void GameTaskScheduler::Process::AddStarvationCount()
    {
        ++_starvationCount;
    }

    auto GameTaskScheduler::Process::ReleaseTask() -> UniquePtrNotNull<GameTask>
    {
        assert(HasTask());

        return std::move(_task);
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

    auto GameTaskScheduler::Process::GetPriority() const -> int64_t
    {
        return _priority;
    }

    void GameTaskScheduler::Process::SetId(int64_t id)
    {
        _id = id;
    }

    void GameTaskScheduler::Process::SetTask(std::unique_ptr<GameTask> task)
    {
        _task = std::move(task);
    }

    void GameTaskScheduler::Process::SetPriority(int64_t value)
    {
        _priority = value;
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

    void GameTaskScheduler::Process::SetTerminateReserved(bool value)
    {
        _terminateReserved = value;
    }

    GameTaskScheduler::GameTaskScheduler(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    GameTaskScheduler::~GameTaskScheduler()
    {
    }

    void GameTaskScheduler::StartDebugOutputLog()
    {
        Post(_gameInstance.GetStrand(), [](WeakPtrNotNull<GameInstance> weak, GameTaskScheduler& self) -> Future<void>
            {
                while (true)
                {
                    constexpr int64_t seconds = 5;

                    co_await Delay(std::chrono::seconds(seconds));

                    auto instance = weak.lock();
                    if (!instance)
                    {
                        co_return;
                    }

                    const int64_t totalTaskCount = self._totalTaskCount;
                    const int64_t completeTaskCount = self.GetCompleteTaskCount();
                    self.ResetCompletionTaskCount();

                    const int64_t runningTaskCount = std::ssize(self.GetProcessesBy(Process::State::Running));
                    const int64_t readyTaskCount = std::ssize(self.GetProcessesBy(Process::State::Ready));

                    ZEROSUGAR_LOG_INFO(instance->GetServiceLocator(),
                        fmt::format("[{}] total_task: {}, complete_task: {:.1f}/s, running_task: {}, ready_task: {}",
                            self.GetName(), totalTaskCount, static_cast<double>(completeTaskCount) / static_cast<double>(seconds), runningTaskCount, readyTaskCount));
                }

            }, _gameInstance.weak_from_this(), std::ref(*this));
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
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (_processTaskQueues.try_emplace(id.Unwrap(), std::queue<UniquePtrNotNull<GameTask>>()).second)
        {
            Process& process = CreateProcess(id.Unwrap());

            process.SetTaskQueueId(id.Unwrap());
        }
        else
        {
            assert(false);
        }
    }

    void GameTaskScheduler::RemoveController(game_controller_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (const auto iter = _processes.find(id.Unwrap()); iter != _processes.end())
        {
            Process& process = iter->second;
            if (process.GetState() == Process::State::Running)
            {
                process.SetTerminateReserved(true);
            }
            else
            {
                ExitProcess(process);
            }
        }
        else
        {
            assert(false);
        }
    }

    void GameTaskScheduler::AddEntity(game_entity_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        [[maybe_unused]]
        const bool inserted = _resources.try_emplace(id.Unwrap(), Resource{}).second;
        assert(inserted);
    }

    void GameTaskScheduler::RemoveEntity(game_entity_id_type id)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        [[maybe_unused]]
        const size_t erased = _resources.erase(id.Unwrap());
        assert(erased);
    }

    void GameTaskScheduler::Schedule(std::unique_ptr<GameTask> task, std::optional<game_controller_id_type> controllerId)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        const std::optional<int64_t> processId = controllerId.has_value() ? controllerId->Unwrap() : std::optional<int64_t>();

        if (_shutdown)
        {
            return;
        }

        if (!Prepare(*task))
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
                ZEROSUGAR_LOG_WARN(_gameInstance.GetServiceLocator(),
                    fmt::format("[{}] fail to find controller. task is dropped. controller_id: {}",
                        GetName(), *controllerId));

                return;
            }
        }

        if (!process)
        {
            process = &CreateProcess(--_nextTempProcessId);
        }

        ++_totalTaskCount;

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

            ChangeState(*process, Process::State::Ready, true, std::move(task));

            if (CanStart(*process))
            {
                ChangeState(*process, Process::State::Running, false, {});

                Start(*process);
            }
        }
        break;
        case Process::State::Count:
        default:
            assert(false);
        }
    }

    auto GameTaskScheduler::GetCompleteTaskCount() const -> int64_t
    {
        return _completeTaskCount.load();
    }

    void GameTaskScheduler::ResetCompletionTaskCount()
    {
        _completeTaskCount.store(0);
    }

    auto GameTaskScheduler::GetName() const -> std::string
    {
        return fmt::format("task_scheduler:{}", _gameInstance.GetId());
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

    bool GameTaskScheduler::Prepare(GameTask& task) const
    {
        if (task.ShouldPrepareBeforeScheduled())
        {
            bool quickExit = false;
            task.Prepare(_gameInstance.GetSerialContext(), quickExit);

            if (quickExit)
            {
                return false;
            }
        }

        if (!task.SelectTargetIds(_gameInstance.GetSerialContext()))
        {
            task.OnFailTargetSelect(_gameInstance.GetSerialContext());

            return false;
        }

        return true;
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

        const int64_t maxConcurrency = std::max<int64_t>(1, _gameInstance.GetExecutor().GetConcurrency());
        int64_t concurrency = std::ssize(GetProcessesBy(Process::State::Running));

        auto iter = readyProcesses.begin();

        // iter can be invalidated. so can't use ++iter
        auto next = iter == readyProcesses.end() ? iter : std::next(iter);

        while (iter != readyProcesses.end())
        {
            if (concurrency >= maxConcurrency)
            {
                break;
            }

            Process* process = *iter;
            assert(process->HasTask() && process->GetState() == Process::State::Ready);

            if (CanStart(*process))
            {
                ChangeState(*process, Process::State::Running, false, {});

                Start(*process);

                ++concurrency;
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

                Post(_gameInstance.GetStrand(), [this, process]()
                    {
                        UniquePtrNotNull<GameTask> task = process->ReleaseTask();
                        DeallocateResource(*process, *task);

                        task->Complete(_gameInstance.GetSerialContext());

                        OnComplete(*process);
                    });
            });
    }

    void GameTaskScheduler::OnComplete(Process& process)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));
        assert(process.GetState() == Process::State::Running);
        assert(!process.HasTask());

        --_totalTaskCount;
        ++_completeTaskCount;

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

        if (taskQueue && !process.IsTerminateReserved())
        {
            if (taskQueue->empty())
            {
                ChangeState(process, Process::State::Waiting, true, std::unique_ptr<GameTask>());
            }
            else
            {
                auto newTask = std::move(taskQueue->front());
                taskQueue->pop();

                ChangeState(process, Process::State::Ready, true, std::move(newTask));
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
                if (iter == _resources.end())
                {
                    return;
                }

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

    void GameTaskScheduler::DeallocateResource(const Process& process, const GameTask& task)
    {
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
                (void)process;

                resource.state = Resource::StateType::Free;
                resource.acquired = nullptr;
            };

        for (int64_t id : task.GetTargetIds())
        {
            deallocate(id);
        }
    }

    void GameTaskScheduler::ChangeState(Process& process, Process::State newState, bool setTask, std::unique_ptr<GameTask> task)
    {
        assert(process.GetState() != newState);

        set_type& olds = GetProcessesBy(process.GetState());
        set_type& news = GetProcessesBy(newState);

        assert(olds.contains(&process));

        auto handle = olds.extract(&process);
        assert(handle);

        if (setTask)
        {
            if (task)
            {
                // To maintain compare order so ensure it can be found, 'process.SetPriority' must be called in place after olds.extract()
                process.SetPriority(task->GetCreationTimePoint().time_since_epoch().count());
            }

            process.SetTask(std::move(task));
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
        const int64_t id = ++_nextRecycleProcessId;

        set_type& states = GetProcessesBy(process.GetState());
        if (auto handle = states.extract(&process); handle)
        {
            if (!_statesRecycleBuffer.contains(handle.value()))
            {
                [[maybe_unused]]
                const bool inserted = _statesRecycleBuffer.insert(std::move(handle)).inserted;
                assert(inserted);
            }
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
