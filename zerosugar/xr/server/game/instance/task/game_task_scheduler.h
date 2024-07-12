#pragma once
#include <boost/unordered/unordered_flat_set.hpp>
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::xr
{
    class GameTask;
    class GameInstance;
}

namespace zerosugar::xr
{
    class GameTaskScheduler final
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
            bool IsTerminateReserved() const;

            void AddStarvationCount();

            auto GetId() const -> int64_t;
            auto GetTask() const -> GameTask&;
            auto GetTaskQueueId() const -> std::optional<int64_t>;
            auto GetState() const -> State;
            auto GetStarvationCount() const -> int64_t;

            void SetId(int64_t id);
            void SetTask(std::unique_ptr<GameTask> task);
            void SetTaskQueueId(std::optional<int64_t> id);
            void SetState(State state);
            void SetStarvationCount(int64_t value);
            void SetTerminateReserved(bool value);

        private:
            int64_t _id = 0;
            std::unique_ptr<GameTask> _task;
            std::optional<int64_t> _taskQueueId = std::nullopt;

            State _state = State::Waiting;
            int64_t _starvationCount = 0;
            bool _terminateReserved = false;
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

            boost::unordered::unordered_flat_set<const Process*> reserved;
        };

    public:
        GameTaskScheduler() = delete;
        explicit GameTaskScheduler(GameInstance& gameInstance);
        ~GameTaskScheduler();

        void Shutdown();
        auto Join() -> Future<void>;

        void AddController(game_controller_id_type id);
        void RemoveController(game_controller_id_type id);

        void AddEntity(game_entity_id_type id);
        void RemoveEntity(game_entity_id_type id);

        void Schedule(std::unique_ptr<GameTask> task, std::optional<game_controller_id_type> controllerId = std::nullopt);


        auto GetCompleteTaskCount() const -> int64_t;
        void ResetCompletionTaskCount();

    private:
        void ScheduleImpl(std::unique_ptr<GameTask> task, std::optional<int64_t> processId);

        auto FindProcess(int64_t id) const -> const Process*;
        auto FindResource(int64_t id) const -> const Resource*;

        bool CanStart(const Process& process) const;

        void TryStartAll();
        void Start(Process& process);

        void OnComplete(Process& process);

        void ReserveResource(const Process& process);
        void AllocateResource(const Process& process);
        void DeallocateResource(const Process& process);

        void ChangeState(Process& process, Process::State newState, std::optional<std::unique_ptr<GameTask>> newTask = std::nullopt);

        auto CreateProcess(int64_t id) -> Process&;
        void ExitProcess(Process& process);

    private:
        using set_type = std::set<Process*, Process::Compare>;

        auto GetProcessesBy(Process::State state) -> set_type&;

    private:
        GameInstance& _gameInstance;

        bool _shutdown = false;
        std::optional<Promise<void>> _shutdownJoinPromise;

        std::atomic<int64_t> _completeTaskCount = 0;

        int64_t _nextTaskQueueId = 0;
        int64_t _nextTempProcessId = -1;
        int64_t _nextRecycleProcessId = 0;

        std::unordered_map<int64_t, Resource> _resources;

        std::unordered_map<int64_t, std::queue<UniquePtrNotNull<GameTask>>> _processTaskQueues;
        std::unordered_map<int64_t, Process> _processes;
        std::unordered_map<int64_t, Process> _processesRecycleBuffer;

        std::array<set_type, static_cast<int32_t>(Process::State::Count)> _states;
        set_type _statesRecycleBuffer;
    };
}
