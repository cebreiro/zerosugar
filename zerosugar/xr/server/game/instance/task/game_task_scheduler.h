#pragma once

namespace zerosugar::xr
{
    class GameTaskBase;

    class GameTaskScheduler final
        : public std::enable_shared_from_this<GameTaskScheduler>
    {
        class Process
        {
        public:
            enum class State
            {
                Running,
                Ready,
                Waiting,

                Count,
            };

        public:
            struct Compare
            {
                bool operator()(PtrNotNull<const Process> lhs, PtrNotNull<const Process> rhs) const;
            };

            Process() = default;
            explicit Process(int64_t id);

            bool HasTask() const;

            void AddStarvationCount();

            auto GetId() const -> int64_t;
            auto GetTask() const -> const GameTaskBase&;
            auto GetTaskQueueId() const -> std::optional<int64_t>;
            auto GetState() const -> State;
            auto GetStarvationCount() const -> int64_t;

            void SetId(int64_t id);
            void SetTask(std::unique_ptr<GameTaskBase> task);
            void SetTaskQueueId(std::optional<int64_t> id);
            void SetState(State state);
            void SetStarvationCount(int64_t value);

        private:
            int64_t _id = 0;
            std::unique_ptr<GameTaskBase> _task;
            std::optional<int64_t> _taskQueueId = std::nullopt;

            State _state = State::Waiting;
            int64_t _starvationCount = 0;
        };

        struct Resource
        {
            enum class StateType
            {
                Free,
                Assigned,
            };

            StateType state = StateType::Free;
            const Process* acquired = nullptr;
            std::vector<const Process*> reservers;
        };

    public:
        GameTaskScheduler() = delete;
        explicit GameTaskScheduler(SharedPtrNotNull<Strand> strand);

        auto AddProcess() -> Future<int64_t>;
        auto RemoveProcess(int64_t id) -> Future<bool>;

        auto AddResource(int64_t id) -> Future<bool>;
        auto RemoveResource(int64_t id) -> Future<bool>;

        void Schedule(std::unique_ptr<GameTaskBase> task, std::optional<int64_t> processId = std::nullopt);

    private:
        void ScheduleImpl(std::unique_ptr<GameTaskBase> task, std::optional<int64_t> processId);

        auto FindProcess(int64_t id) const -> const Process*;
        auto FindResource(int64_t id) const -> const Resource*;

        bool CanStart(const Process& process) const;

        void OnComplete(Process& process);

        void TryStartAll();
        void Start(Process& process);

        void ReserveResource(const Process& process);
        void AllocateResource(const Process& process);
        void DeallocateResource(const Process& process);

        void ChangeState(Process& process, Process::State newState);

        auto CreateProcess(int64_t id) -> Process&;
        void ExitProcess(Process& process);

    private:
        using set_type = std::set<Process*, Process::Compare>;

        auto GetProcessesBy(Process::State state) -> set_type&;

    private:
        SharedPtrNotNull<Strand> _strand;

        int64_t _nextTaskQueueId = 0;
        int64_t _nextProcessId = 0;
        int64_t _nextRecycleProcessId = 0;

        std::unordered_map<int64_t, Resource> _resources;

        std::unordered_map<int64_t, std::queue<std::unique_ptr<GameTaskBase>>> _processTaskQueues;
        std::unordered_map<int64_t, Process> _processes;
        std::unordered_map<int64_t, Process> _processesRecycleBuffer;

        std::array<set_type, static_cast<int32_t>(Process::State::Count)> _states;
        set_type _statesRecycleBuffer;
    };
}
