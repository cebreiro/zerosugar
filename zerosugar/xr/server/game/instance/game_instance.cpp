#include "game_instance.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/controller/game_entity_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/view/game_view_model_container.h"
#include "zerosugar/xr/server/game/instance/view/game_player_view_model.h"
#include "zerosugar/xr/server/game/instance/view/grid/game_spatial_container.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/view/game_view_controller.h"
#include "zerosugar/xr/server/game/packet/packet_builder.h"

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
        , _gameViewController(std::make_unique<GameViewController>(*this))
        , _viewModelContainer(std::make_unique<GameViewModelContainer>())
    {
    }

    GameInstance::~GameInstance()
    {
    }

    auto GameInstance::SpawnEntity(SharedPtrNotNull<GameEntity> entity) -> Future<void>
    {
        // TODO: transform to player spawn task 
        [[maybe_unused]]
        auto self = shared_from_this();

        if (!ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        entity->SetId(game_entity_id_type(GameEntityType::Player, ++_nextPlayerId));

        bool result = _entityContainer->Add(entity);
        assert(result);

        auto playerView = new GamePlayerViewModel(entity->GetController());
        playerView->Initialize(*entity);

        result = _viewModelContainer->Add(std::unique_ptr<GamePlayerViewModel>(playerView));
        assert(result);

        result = co_await _taskScheduler->AddProcess(entity->GetController().GetControllerId());
        assert(result);

        const MovementComponent& movementComponent = entity->GetComponent<MovementComponent>();

        GameSpatialSector& sector = _spatialContainer->GetSector(movementComponent.GetX(), movementComponent.GetY());

        if (entity->GetController().IsSubscriberOf(network::game::sc::EnterGame::opcode))
        {
            network::game::sc::EnterGame packet;
            GamePacketBuilder::Build(packet, *this, *entity, sector);

            entity->GetController().Notify(packet);
        }

        if (auto range = sector.GetEntities(GameEntityType::Player); range.begin() != range.end())
        {
            network::game::sc::AddRemotePlayer packet;
            GamePacketBuilder::Build(packet, *playerView);

            for (const game_entity_id_type id : range)
            {
                GamePlayerViewModel* remote = _viewModelContainer->FindPlayer(id);
                assert(remote);

                remote->GetController().Notify(packet);
            }
        }

        sector.AddEntity(entity->GetId());
    }

    void GameInstance::Summit(UniquePtrNotNull<GameTask> task, std::optional<int64_t> controllerId)
    {
        Dispatch(*_strand, [self = shared_from_this(), task = std::move(task), controllerId]() mutable
            {
                task->SelectTargetIds(self->GetSerialContext());

                self->_taskScheduler->Schedule(std::move(task), controllerId);
            });
    }

    auto GameInstance::PublishControllerId() -> int64_t
    {
        return _nextControllerId.fetch_add(1);
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

    auto GameInstance::GetViewController() -> GameViewController&
    {
        return *_gameViewController;
    }

    auto GameInstance::GetViewModelContainer() -> GameViewModelContainer&
    {
        return *_viewModelContainer;
    }

    auto GameInstance::GetViewModelContainer() const -> const GameViewModelContainer&
    {
        return *_viewModelContainer;
    }
}
