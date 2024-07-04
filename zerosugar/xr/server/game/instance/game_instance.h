#pragma once
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/game_instance_id.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr::data
{
    struct Map;
}

namespace zerosugar::xr
{
    class GameEntity;
    class GameEntityContainer;

    class GameSpatialContainer;
    class GameSnapshotContainer;
    class GameSnapshotView;
    class GameSnapshotController;

    class GameTask;
    class GameTaskScheduler;

    class AIControlService;
}

namespace zerosugar::xr
{
    class GameInstance final : public std::enable_shared_from_this<GameInstance>
    {
    public:
        GameInstance(SharedPtrNotNull<execution::IExecutor> executor, const ServiceLocator& serviceLocator,
            game_instance_id_type id, int32_t zoneId);
        ~GameInstance();

        void Start();
        void Summit(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId);

        auto PublishControllerId() -> game_controller_id_type;
        auto PublishEntityId(GameEntityType type) -> game_entity_id_type;

    public:
        auto GetExecutor() const -> execution::IExecutor&;
        auto GetStrand() const -> Strand&;
        auto GetServiceLocator() -> ServiceLocator&;

        auto GetId() const -> game_instance_id_type;
        auto GetZoneId() const -> int32_t;
        auto GetMapData() const -> const data::Map&;

        auto GetParallelContext() -> GameExecutionParallel&;
        auto GetParallelContext() const -> const GameExecutionParallel&;

        auto GetSerialContext() -> GameExecutionSerial&;
        auto GetSerialContext() const -> const GameExecutionSerial&;

        auto GetTaskScheduler() -> GameTaskScheduler&;
        auto GetTaskScheduler() const -> const GameTaskScheduler&;

        auto GetEntityContainer() -> GameEntityContainer&;
        auto GetEntityContainer() const -> const GameEntityContainer&;

        auto GetSpatialContainer() -> GameSpatialContainer&;
        auto GetSpatialContainer() const -> const GameSpatialContainer&;

        auto GetSnapshotContainer() -> GameSnapshotContainer&;
        auto GetSnapshotContainer() const -> const GameSnapshotContainer&;

        auto GetSnapshotController() -> GameSnapshotController&;
        auto GetSnapshotView() -> GameSnapshotView&;

        auto GetAIControlService() -> AIControlService&;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;
        ServiceLocator _serviceLocator;

        game_instance_id_type _id;
        int32_t _zoneId = 0;
        const data::Map* _data = nullptr;

        std::atomic<int64_t> _nextControllerId = 0;
        std::array<std::atomic<int32_t>, static_cast<int32_t>(GameEntityType::Count)> _nextEntityIds = {};

        GameExecutionParallel _parallel;
        GameExecutionSerial _serial;

        UniquePtrNotNull<GameTaskScheduler> _taskScheduler;
        UniquePtrNotNull<GameEntityContainer> _entityContainer;

        UniquePtrNotNull<GameSpatialContainer> _spatialContainer;
        UniquePtrNotNull<GameSnapshotContainer> _snapshotContainer;
        UniquePtrNotNull<GameSnapshotView> _snapshotView;
        UniquePtrNotNull<GameSnapshotController> _snapshotController;

        UniquePtrNotNull<AIControlService> _aiControlService;
    };
}
