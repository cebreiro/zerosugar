#pragma once
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr::coordination
{
    class NodeContainer;
}

namespace zerosugar::xr
{
    class CoordinationService final
        : public service::ICoordinationService
        , public std::enable_shared_from_this<CoordinationService>
    {
    public:
        explicit CoordinationService(SharedPtrNotNull<execution::IExecutor> executor);
        ~CoordinationService();

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        auto RegisterServerAsync(service::RegisterServerParam param) -> Future<service::RegisterServerResult> override;
        auto OpenChannelAsync(AsyncEnumerable<service::CoordinationChannelInput> param)
            -> AsyncEnumerable<service::CoordinationChannelOutput> override;

        auto RequestSnowflakeKeyAsync(service::RequestSnowflakeKeyParam param) -> Future<service::RequestSnowflakeKeyResult> override;
        auto ReturnSnowflakeKeyAsync(service::ReturnSnowflakeKeyParam param) -> Future<service::ReturnSnowflakeKeyResult> override;

        auto AddPlayerAsync(service::AddPlayerParam param) -> Future<service::AddPlayerResult> override;

    private:
        auto PublishSnowflakeKey(const std::string& requester) -> std::optional<int32_t>;

    private:
        SharedPtrNotNull<execution::IExecutor> _executor;
        SharedPtrNotNull<Strand> _strand;

        ServiceLocator _serviceLocator;

        int32_t _nextSnowflake = 0;
        std::unordered_multimap<std::string, int32_t> _publishedSnowflakeKeys;
        std::queue<int32_t> _returnedSnowflakeKeys;

        int64_t _nextServerId = 0;

        std::optional<std::pair<std::string, int32_t>> _lobby;
        std::unique_ptr<coordination::NodeContainer> _nodeContainer;
    };
}
