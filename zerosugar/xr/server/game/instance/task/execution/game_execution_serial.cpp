#include "game_execution_serial.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr
{
    GameExecutionSerial::GameExecutionSerial(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
    {
    }

    GameExecutionSerial::~GameExecutionSerial()
    {
    }

    auto GameExecutionSerial::Hold() -> std::shared_ptr<GameExecutionSerial>
    {
        return std::shared_ptr<GameExecutionSerial>(_gameInstance.shared_from_this(), this);
    }

    void GameExecutionSerial::SummitTask(UniquePtrNotNull<GameTask> task, std::optional<game_controller_id_type> controllerId)
    {
        _gameInstance.Summit(std::move(task), controllerId);
    }

    auto GameExecutionSerial::PublishEntityId(GameEntityType type) -> game_entity_id_type
    {
        return _gameInstance.PublishEntityId(type);
    }

    auto GameExecutionSerial::GetServiceLocator() const -> ServiceLocator&
    {
        return _gameInstance.GetServiceLocator();
    }

    auto GameExecutionSerial::GetMapData() const -> const data::Map&
    {
        return _gameInstance.GetMapData();
    }

    auto GameExecutionSerial::GetTaskScheduler() -> GameTaskScheduler&
    {
        return _gameInstance.GetTaskScheduler();
    }

    auto GameExecutionSerial::GetTaskScheduler() const -> const GameTaskScheduler&
    {
        return _gameInstance.GetTaskScheduler();
    }

    auto GameExecutionSerial::GetSnapshotController() -> GameSnapshotController&
    {
        return _gameInstance.GetSnapshotController();
    }

    auto GameExecutionSerial::GetSnapshotView() -> GameSnapshotView&
    {
        return _gameInstance.GetSnapshotView();
    }

    auto GameExecutionSerial::GetSnapshotContainer() -> GameSnapshotContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetSnapshotContainer();
    }

    auto GameExecutionSerial::GetSnapshotContainer() const -> const GameSnapshotContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetSnapshotContainer();
    }

    auto GameExecutionSerial::GetSpatialContainer() -> GameSpatialContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetSpatialContainer();
    }

    auto GameExecutionSerial::GetSpatialContainer() const -> const GameSpatialContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetSpatialContainer();
    }

    auto GameExecutionSerial::GetAIControlService() -> AIControlService&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetAIControlService();
    }

    auto GameExecutionSerial::GetNavigationService() -> NavigationService*
    {
        return _gameInstance.GetNavigationService();
    }

    auto GameExecutionSerial::GetGMCommandFactory() const -> const IGMCommandFactory&
    {
        return _gameInstance.GetGMCommandFactory();
    }
}
