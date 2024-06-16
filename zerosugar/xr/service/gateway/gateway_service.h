#pragma once
#include <tbb/concurrent_unordered_map.h>
#include "zerosugar/xr/service/model/generated/gateway_service.h"

namespace zerosugar::xr
{
    class RPCClient;

    class GatewayService final
        : public service::IGatewayService
        , public std::enable_shared_from_this<GatewayService>
    {
    public:
        explicit GatewayService(SharedPtrNotNull<execution::IExecutor> executor);

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto AddGameServiceAsync(service::AddGameServiceParam param) -> Future<service::AddGameServiceResult> override;
        auto GetGameServiceListAsync(service::GetGameServiceListParam param) -> Future<service::GetGameServiceListResult> override;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;

        tbb::concurrent_unordered_map<std::string, std::pair<std::string, int32_t>> _gameServices;
    };
}
