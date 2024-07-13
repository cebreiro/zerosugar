#include "bot_control_service.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/shared/ai/behavior_tree/log/behavior_tree_log_service_adapter.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/socket.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/register_task.h"

namespace zerosugar::xr
{
    BotControlService::BotControlService(const ServiceLocator& locator,
        SharedPtrNotNull<execution::AsioExecutor> ioExecutor, SharedPtrNotNull<execution::IExecutor> gameExecutor,
        int64_t concurrency, int64_t botCount, const std::string& btName)
        : _serviceLocator(locator)
        , _ioExecutor(std::move(ioExecutor))
        , _gameExecutor(std::move(gameExecutor))
        , _concurrency(std::max<int64_t>(1, concurrency))
        , _botControllers(std::max<int64_t>(1, botCount))
        , _behaviorTreeLogger(std::make_unique<BehaviorTreeLogServiceAdapter>(_serviceLocator.Get<ILogService>(), LogLevel::Debug))
        , _nodeSerializer(std::make_unique<bt::NodeSerializer>())
    {
        bot::RegisterTask(*_nodeSerializer);

        _sharedContexts.resize(_concurrency);

        for (int64_t i = 0; i < concurrency; ++i)
        {
            _sharedContexts[i].naviStrand = std::make_shared<Strand>(_gameExecutor);
        }

        for (int64_t i = 0; i < std::ssize(_botControllers); ++i)
        {
            _botControllers[i] = std::make_shared<BotController>(_serviceLocator,
                std::make_shared<Strand>(_gameExecutor),
                _sharedContexts[i % concurrency],
                std::make_shared<Socket>(_ioExecutor->MakeStrand()),
                i,
                *_nodeSerializer,
                btName);

            _botControllers[i]->SetLogger(_behaviorTreeLogger.get());
        }
    }

    BotControlService::~BotControlService()
    {
    }

    void BotControlService::Start()
    {
        for (const SharedPtrNotNull<BotController>& botController : _botControllers)
        {
            botController->Start();
        }
    }

    auto BotControlService::GetName() const -> std::string_view
    {
        return "bot_control_service";
    }
}
