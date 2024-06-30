#include "game_execution_serial.h"

#include "zerosugar/xr/server/game/instance/game_instance.h"

namespace zerosugar::xr
{
    GameExecutionSerial::GameExecutionSerial(GameInstance& gameInstance)
        : _gameInstance(gameInstance)
        , _serviceLocator(std::make_unique<service_locator_type>(_gameInstance.GetServiceLocator()))
    {
    }

    auto GameExecutionSerial::GetServiceLocator() const -> service_locator_type&
    {
        return *_serviceLocator;
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

    auto GameExecutionSerial::GetSnapshotContainer() -> GameSnapshotModelContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetSnapshotContainer();
    }

    auto GameExecutionSerial::GetSnapshotContainer() const -> const GameSnapshotModelContainer&
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
}
