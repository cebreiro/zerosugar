#pragma once
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"

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
        BotControlService(SharedPtrNotNull<execution::AsioExecutor> executor, const ServiceLocator& locator,
            int64_t concurrency, int64_t botCount, const std::string& btName);
        ~BotControlService();

        void Start();

        auto GetName() const -> std::string_view override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        ServiceLocator _serviceLocator;

        std::vector<SharedPtrNotNull<execution::AsioStrand>> _strands;
        std::vector<SharedPtrNotNull<BotController>> _botControllers;

        std::unique_ptr<bt::NodeSerializer> _nodeSerializer;
    };
}
