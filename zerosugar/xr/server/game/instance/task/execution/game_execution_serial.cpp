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

    auto GameExecutionSerial::GetViewController() -> GameViewController&
    {
        return _gameInstance.GetViewController();
    }

    auto GameExecutionSerial::GetViewModelContainer() -> GameViewModelContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetViewModelContainer();
    }

    auto GameExecutionSerial::GetViewModelContainer() const -> const GameViewModelContainer&
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        return _gameInstance.GetViewModelContainer();
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