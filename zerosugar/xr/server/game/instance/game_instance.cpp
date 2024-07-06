#include "game_instance.h"

#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/data/provider/map_data_provider.h"
#include "zerosugar/xr/data/provider/navigation_data_provider.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/ai/ai_control_service.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_spawner_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/snapshot/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/impl/spawner_install.h"

namespace zerosugar::xr
{
    GameInstance::GameInstance(SharedPtrNotNull<execution::IExecutor> executor, const ServiceLocator& serviceLocator,
        game_instance_id_type id, int32_t zoneId)
        : _executor(std::move(executor))
        , _strand(std::make_shared<Strand>(_executor))
        , _serviceLocator(serviceLocator)
        , _id(id)
        , _zoneId(zoneId)
        , _parallel(*this)
        , _serial(*this)
        , _taskScheduler(std::make_unique<GameTaskScheduler>(*this))
        , _entityContainer(std::make_unique<GameEntityContainer>())
        , _spatialContainer(std::make_unique<GameSpatialContainer>(100000, 100000, 300))
        , _snapshotContainer(std::make_unique<GameSnapshotContainer>())
        , _snapshotView(std::make_unique<GameSnapshotView>(*this))
        , _snapshotController(std::make_unique<GameSnapshotController>(*this))
        , _aiControlService(std::make_unique<AIControlService>(*this))
    {
        const GameDataProvider& gameDataProvider = serviceLocator.Get<GameDataProvider>();
        const MapDataProvider& mapDataProvider = gameDataProvider.GetMapDataProvider();

        _data = mapDataProvider.Find(_zoneId);
        assert(_data);

        if (_data->type != data::MapType::Village)
        {
            const NavigationDataProvider& naviDataProvider = gameDataProvider.GetNavigationDataProvider();
            navi::Data naviData = naviDataProvider.Create(_data->id);

            _navigationService = std::make_shared<NavigationService>(_serviceLocator,
                std::make_shared<Strand>(_executor), std::move(naviData));

            _navigationService->StartVisualize();
        }
    }

    GameInstance::~GameInstance()
    {
        _taskScheduler->Shutdown();
        _taskScheduler->Join();
    }

    void GameInstance::Start()
    {
        for (const data::MonsterSpawner& spawnData : _data->monsterSpawners)
        {
            Summit(std::make_unique<game_task::SpawnerInstall>(&spawnData), std::nullopt);
        }
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

    auto GameInstance::PublishEntityId(GameEntityType type) -> game_entity_id_type
    {
        const int32_t index = static_cast<int32_t>(type);
        assert(index >= 0 && index < std::ssize(_nextEntityIds));

        return game_entity_id_type(type, _nextEntityIds[index].fetch_add(1));
    }

    auto GameInstance::GetExecutor() const -> execution::IExecutor&
    {
        return *_executor;
    }

    auto GameInstance::GetStrand() const -> Strand&
    {
        return *_strand;
    }

    auto GameInstance::GetServiceLocator() -> ServiceLocator&
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

    auto GameInstance::GetMapData() const -> const data::Map&
    {
        assert(_data);

        return *_data;
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

    auto GameInstance::GetSnapshotContainer() -> GameSnapshotContainer&
    {
        return *_snapshotContainer;
    }

    auto GameInstance::GetSnapshotContainer() const -> const GameSnapshotContainer&
    {
        return *_snapshotContainer;
    }

    auto GameInstance::GetAIControlService() -> AIControlService&
    {
        return *_aiControlService;
    }

    auto GameInstance::GetNavigationService() -> NavigationService*
    {
        return _navigationService.get();
    }
}
