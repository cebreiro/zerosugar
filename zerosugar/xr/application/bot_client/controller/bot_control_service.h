#pragma once
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/xr/application/bot_client/controller/bot_shared_context.h"

namespace zerosugar
{
    class IBehaviorTreeLogger;
}

namespace zerosugar::bt
{
    class NodeSerializer;
}

namespace zerosugar::xr
{
    class BotController;
}

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class BotControlService : public IService
    {
    public:
        BotControlService(const ServiceLocator& locator,
            SharedPtrNotNull<execution::AsioExecutor> executor, SharedPtrNotNull<execution::IExecutor> gameExecutor,
            int64_t concurrency, int64_t botCount, const std::string& btName);
        ~BotControlService();

        void Start();

        auto GetName() const -> std::string_view override;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<execution::AsioExecutor> _ioExecutor;
        SharedPtrNotNull<execution::IExecutor> _gameExecutor;

        std::vector<bot::SharedContext> _sharedContexts;
        std::vector<std::shared_ptr<Strand>> _strands;
        std::vector<SharedPtrNotNull<BotController>> _botControllers;

        UniquePtrNotNull<IBehaviorTreeLogger> _behaviorTreeLogger;
        UniquePtrNotNull<bt::NodeSerializer> _nodeSerializer;
    };
}
