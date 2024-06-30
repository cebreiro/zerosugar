#include "game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"

namespace zerosugar::xr
{
    GameInstance::GameInstance(SharedPtrNotNull<execution::IExecutor> executor, service_locator_type serviceLocator,
        game_instance_id_type id, int32_t zoneId)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
        , _serviceLocator(std::move(serviceLocator))
        , _id(id)
        , _zoneId(zoneId)
        , _parallel(*this)
        , _serial(*this)
        , _taskScheduler(std::make_unique<GameTaskScheduler>(*this))
        , _entityContainer(std::make_unique<GameEntityContainer>())
        , _spatialContainer(std::make_unique<GameSpatialContainer>(100000, 100000, 300))
        , _snapshotContainer(std::make_unique<GameSnapshotModelContainer>())
        , _snapshotView(std::make_unique<GameSnapshotView>(*this))
        , _snapshotController(std::make_unique<GameSnapshotController>(*this))
    {
    }

    GameInstance::~GameInstance()
    {
        _taskScheduler->Shutdown();
        _taskScheduler->Join();
    }

    void GameInstance::Summit(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId)
    {
        assert(task);

        Post(*_strand, [self = shared_from_this(), task = std::move(task), controllerId]() mutable
            {
                if (task->ShouldPrepareBeforeScheduled())
                {
                    bool quickExit = false;
                    task->Prepare(self->GetSerialContext(), quickExit);

                    if (quickExit)
                    {
                        return;
                    }
                }

                if (!task->SelectTargetIds(self->GetSerialContext()))
                {
                    return;
                }

                self->_taskScheduler->Schedule(std::move(task), controllerId);
            });
    }

    auto GameInstance::PublishControllerId() -> game_controller_id_type
    {
        return game_controller_id_type(_nextControllerId.fetch_add(1));
    }

    auto GameInstance::PublishPlayerId() -> game_entity_id_type
    {
        return game_entity_id_type(GameEntityType::Player, _nextPlayerId.fetch_add(1));
    }

    auto GameInstance::GetExecutor() const -> execution::IExecutor&
    {
        return *_executor;
    }

    auto GameInstance::GetStrand() const -> Strand&
    {
        return *_strand;
    }

    auto GameInstance::GetServiceLocator() -> service_locator_type&
    {
        return _serviceLocator;
    }

    auto GameInstance::GetId() const -> game_instance_id_type
    {
        return _id;
    }

    auto GameInstance::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto GameInstance::GetParallelContext() -> GameExecutionParallel&
    {
        return _parallel;
    }

    auto GameInstance::GetParallelContext() const -> const GameExecutionParallel&
    {
        return _parallel;
    }

    auto GameInstance::GetSerialContext() -> GameExecutionSerial&
    {
        return _serial;
    }

    auto GameInstance::GetSerialContext() const -> const GameExecutionSerial&
    {
        return _serial;
    }

    auto GameInstance::GetTaskScheduler() -> GameTaskScheduler&
    {
        return *_taskScheduler;
    }

    auto GameInstance::GetTaskScheduler() const -> const GameTaskScheduler&
    {
        return *_taskScheduler;
    }

    auto GameInstance::GetEntityContainer() -> GameEntityContainer&
    {
        return *_entityContainer;
    }

    auto GameInstance::GetEntityContainer() const -> const GameEntityContainer&
    {
        return *_entityContainer;
    }

    auto GameInstance::GetSpatialContainer() -> GameSpatialContainer&
    {
        return *_spatialContainer;
    }

    auto GameInstance::GetSpatialContainer() const -> const GameSpatialContainer&
    {
        return *_spatialContainer;
    }

    auto GameInstance::GetSnapshotController() -> GameSnapshotController&
    {
        return *_snapshotController;
    }

    auto GameInstance::GetSnapshotView() -> GameSnapshotView&
    {
        return *_snapshotView;
    }

    auto GameInstance::GetSnapshotContainer() -> GameSnapshotModelContainer&
    {
        return *_snapshotContainer;
    }

    auto GameInstance::GetSnapshotContainer() const -> const GameSnapshotModelContainer&
    {
        return *_snapshotContainer;
    }
}
