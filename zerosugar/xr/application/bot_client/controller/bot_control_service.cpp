#include "bot_control_service.h"

#include "zerosugar/shared/ai/behavior_tree/data/node_serializer.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/behavior_tree/task/register.h"

namespace zerosugar::xr
{
    BotControlService::BotControlService(SharedPtrNotNull<execution::AsioExecutor> executor,
        const ServiceLocator& locator, int64_t concurrency, int64_t botCount, const std::string& btName)
        : _executor(std::move(executor))
        , _serviceLocator(locator)
        , _strands(std::max<int64_t>(1, concurrency))
        , _botControllers(std::max<int64_t>(1, botCount))
        , _nodeSerializer(std::make_unique<bt::NodeSerializer>())
    {
        bot::Register(*_nodeSerializer);

        for (int64_t i = 0; i < std::ssize(_strands); ++i)
        {
            _strands[i] = _executor->MakeStrand();
        }

        for (int64_t i = 0; i < std::ssize(_botControllers); ++i)
        {
            _botControllers[i] = std::make_shared<BotController>(_serviceLocator,
                _strands[i % std::ssize(_strands)], i, *_nodeSerializer, btName);
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
