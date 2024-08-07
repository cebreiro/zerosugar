#include "game_instance.h"

#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_data_provider.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_serializer.h"
#include "zerosugar/xr/server/game/instance/ai/ai_control_service.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_spawner_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_view.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/impl/spawner_install.h"
#include "zerosugar/xr/server/game/instance/gm/gm_command_factory.h"
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"
#include "zerosugar/xr/server/game/instance/grid/game_spatial_scanner.h"

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
        , _spatialContainer(std::make_unique<GameSpatialContainer>(100000, 100000, 2000))
        , _spatialScanner(std::make_unique<GameSpatialScanner>(*this))
        , _snapshotContainer(std::make_unique<GameSnapshotContainer>())
        , _snapshotView(std::make_unique<GameSnapshotView>(*this))
        , _snapshotController(std::make_unique<GameSnapshotController>(*this))
        , _aiControlService(std::make_unique<AIControlService>(*this))
        , _gmCommandFactory(std::make_unique<GMCommandFactory>())
    {
        const GameDataProvider& gameDataProvider = serviceLocator.Get<GameDataProvider>();
        _data = gameDataProvider.Find(map_data_id_type(_zoneId));
        assert(_data);

        if (_data->GetType() != data::MapType::Village)
        {
            const NavigationDataProvider& naviDataProvider = serviceLocator.Get<NavigationDataProvider>();
            navi::Data naviData = naviDataProvider.Create(_data->GetId());

            _navigationService = std::make_shared<NavigationService>(_serviceLocator,
                std::make_shared<Strand>(_executor), std::move(naviData));
        }

        _entitySerializer = std::make_unique<GameEntitySerializer>(*_data);
    }

    GameInstance::~GameInstance()
    {
    }

    auto GameInstance::Start() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        for (const data::MonsterSpawner& spawnData : _data->GetMonsterSpawners())
        {
            Summit(std::make_unique<game_task::SpawnerInstall>(&spawnData), std::nullopt);
        }

        if (_data->GetType() != data::MapType::Village)
        {
            GetTaskScheduler().StartDebugOutputLog();
        }

        co_return;
    }

    void GameInstance::Shutdown()
    {
        Dispatch(*_strand, [self = shared_from_this()]()
            {
                self->_taskScheduler->Shutdown();
                self->_spatialScanner->Shutdown();
            });
    }

    auto GameInstance::Join() -> Future<void>
    {
        auto future1 = _aiControlService->ShutdownAndJoin();
        auto future2 = _taskScheduler->Join();
        auto future3 = _spatialScanner->Join();

        co_await WaitAll(GetExecutor(), future1, future2, future3);

        co_return;
    }

    void GameInstance::Summit(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId)
    {
        assert(task);

        Post(*_strand, [self = shared_from_this(), task = std::move(task), controllerId]() mutable
            {
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

    auto GameInstance::GetMapData() const -> const MapData&
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

    auto GameInstance::GetEntitySerializer() const -> const IGameEntitySerializer&
    {
        return *_entitySerializer;
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

    auto GameInstance::GetSpatialScanner() -> GameSpatialScanner&
    {
        return *_spatialScanner;
    }

    auto GameInstance::GetSpatialScanner() const -> const GameSpatialScanner&
    {
        return *_spatialScanner;
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

    auto GameInstance::GetGMCommandFactory() const -> const IGMCommandFactory&
    {
        return *_gmCommandFactory;
    }
}
