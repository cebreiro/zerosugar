#pragma once
#include "zerosugar/xr/server/game/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/game_instance_id.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"

namespace zerosugar::xr
{
    class GameEntity;
    class GameEntityContainer;

    class GameSpatialContainer;
    class GameSnapshotModelContainer;
    class GameSnapshotView;
    class GameSnapshotController;

    class GameTask;
    class GameTaskScheduler;
}

namespace zerosugar::xr
{
    class GameInstance final : public std::enable_shared_from_this<GameInstance>
    {
    public:
        using service_locator_type = ServiceLocatorT<ILogService, IGameRepository>;

    public:
        GameInstance(SharedPtrNotNull<execution::IExecutor> executor, service_locator_type serviceLocator,
            game_instance_id_type id, int32_t zoneId);
        ~GameInstance();

        void Summit(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId);

        auto PublishControllerId() -> game_controller_id_type;
        auto PublishPlayerId() -> game_entity_id_type;

    public:
        auto GetExecutor() const -> execution::IExecutor&;
        auto GetStrand() const -> Strand&;
        auto GetServiceLocator() -> service_locator_type&;

        auto GetId() const -> game_instance_id_type;
        auto GetZoneId() const -> int32_t;

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

        auto GetSnapshotContainer() -> GameSnapshotModelContainer&;
        auto GetSnapshotContainer() const -> const GameSnapshotModelContainer&;

        auto GetSnapshotController() -> GameSnapshotController&;
        auto GetSnapshotView() -> GameSnapshotView&;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;
        service_locator_type _serviceLocator;

        game_instance_id_type _id;
        int32_t _zoneId = 0;

        std::atomic<int64_t> _nextControllerId = 0;
        std::atomic<int32_t> _nextPlayerId = 0;

        GameExecutionParallel _parallel;
        GameExecutionSerial _serial;

        UniquePtrNotNull<GameTaskScheduler> _taskScheduler;
        UniquePtrNotNull<GameEntityContainer> _entityContainer;

        UniquePtrNotNull<GameSpatialContainer> _spatialContainer;
        UniquePtrNotNull<GameSnapshotModelContainer> _snapshotContainer;
        UniquePtrNotNull<GameSnapshotView> _snapshotView;
        UniquePtrNotNull<GameSnapshotController> _snapshotController;
    };
}
