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
                Blocked,
                Empty,

                Count,
            };

        public:
            struct Compare
            {
                bool operator()(PtrNotNull<const Process> lhs, PtrNotNull<const Process> rhs) const;
            };

            bool Empty() const;

            void Push(std::unique_ptr<GameTaskBase> task);
            void Pop();

            auto Front() -> GameTaskBase&;
            auto Front() const -> const GameTaskBase&;

            void AddStarvationCount();

            auto GetState() const -> State;
            auto GetStarvationCount() const -> int64_t;

            void SetState(State state);
            void ResetStarvationCount();

        private:
            friend bool operator<(const Process& lhs, const Process& rhs);

        private:
            std::vector<std::unique_ptr<GameTaskBase>> _queue;
            int64_t _starvationCount = 0;
        };

        enum class ResourceStateType
        {
            Released,
            Acquired,
        };

        struct Resource
        {
            ResourceStateType state = ResourceStateType::Released;
            const Process* acquiredProcess = nullptr;
            std::vector<const Process*> reservedProcesses;
        };

    public:
        bool AddProcess(int64_t processId);
        bool RemoveProcess(int64_t processId);

        void Schedule(std::unique_ptr<GameTaskBase> task);

    private:
        void ScheduleImpl(std::unique_ptr<GameTaskBase> task);

        auto FindProcess(int64_t id) const -> const Process*;
        auto FindOwnership(int64_t id) const -> const Resource*;

        bool CanSchedule(const Process& process) const;

        void OnComplete(Process& process);

        void TryScheduleAll();

        void StartTask(Process& process);

        void ReserveResource(const Process& process);
        void ReserveResource(int64_t id, const Process& process);

        void GiveResource(Process& process);
        void GiveResource(int64_t id, const Process& process);

        void TakeBackResource(Process& process);
        void TakeBackResource(int64_t id);

        void ChangeProcessState(Process& process, Process::State newState);

    private:
        using map_type = std::map<Process*, int8_t, Process::Compare>;

        auto GetProcessStates(Process::State state) -> map_type&;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;

        std::unordered_map<int64_t, Process> _processes;
        std::unordered_map<int64_t, Resource> _resources;

        std::array<map_type, static_cast<int32_t>(Process::State::Count)> _states;
    };
}
