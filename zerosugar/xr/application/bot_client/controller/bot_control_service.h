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
    class BotControlService final
        : public IService
        , public std::enable_shared_from_this<BotControlService>
    {
    public:
        BotControlService(const ServiceLocator& locator,
            SharedPtrNotNull<execution::AsioExecutor> executor, SharedPtrNotNull<execution::IExecutor> gameExecutor,
            int64_t concurrency, int64_t botCount, const std::string& btName);
        ~BotControlService();

        void Start();

        auto GetName() const -> std::string_view override;

    private:
        struct RTTStatistics
        {
            int64_t min = std::numeric_limits<int64_t>::max();
            int64_t max = std::numeric_limits<int64_t>::min();
            int64_t average = -1;
        };

        auto EvaluateRTTStatistics() -> Future<RTTStatistics>;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<execution::AsioExecutor> _ioExecutor;
        SharedPtrNotNull<execution::IExecutor> _gameExecutor;

        int64_t _concurrency = 1;

        std::vector<bot::SharedContext> _sharedContexts;
        std::vector<SharedPtrNotNull<BotController>> _botControllers;

        UniquePtrNotNull<IBehaviorTreeLogger> _behaviorTreeLogger;
        UniquePtrNotNull<bt::NodeSerializer> _nodeSerializer;

        int64_t _pingSequence = 0;
        std::vector<Future<std::optional<std::chrono::system_clock::duration>>> _pingOperations;
    };
}
