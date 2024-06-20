#pragma once
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr::coordination
{
    class NodeContainer;
    class ILoadBalancer;
    class CommandResponseHandlerFactory;
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
        auto UpdateServerStatusAsync(service::UpdateServerStatusParam param)
            -> Future<service::UpdateServerStatusResult> override;
        auto OpenChannelAsync(AsyncEnumerable<service::CoordinationCommandResponse> param)
            -> AsyncEnumerable<service::CoordinationCommand> override;

        auto RequestSnowflakeKeyAsync(service::RequestSnowflakeKeyParam param) -> Future<service::RequestSnowflakeKeyResult> override;
        auto ReturnSnowflakeKeyAsync(service::ReturnSnowflakeKeyParam param) -> Future<service::ReturnSnowflakeKeyResult> override;

        auto AddPlayerAsync(service::AddPlayerParam param) -> Future<service::AddPlayerResult> override;

    public:
        auto GetStrand() -> Strand&;
        auto GetServiceLocator() -> ServiceLocator&;

        auto GetNodeContainer() -> coordination::NodeContainer&;
        auto GetChannelInputHandlerFactory() -> const coordination::CommandResponseHandlerFactory&;

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
        int64_t _nextGameInstanceId = 0;

        std::optional<std::pair<std::string, int32_t>> _lobby;
        std::unique_ptr<coordination::NodeContainer> _nodeContainer;
        std::unique_ptr<coordination::ILoadBalancer> _loadBalancer;
        std::unique_ptr<coordination::CommandResponseHandlerFactory> _commandResponseHandlerFactory;
    };
}
