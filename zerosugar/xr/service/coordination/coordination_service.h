#pragma once
#include "zerosugar/xr/service/coordination/node/node_id.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr::coordination
{
    class GameUser;
    class NodeContainer;
    class ILoadBalancer;
    class CommandResponseHandlerFactory;
}

namespace zerosugar::xr
{
    class DungeonMatchGroup;
    class DungeonMatchCoordinator;
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
        auto AuthenticatePlayerAsync(service::AuthenticatePlayerParam param) -> Future<service::AuthenticatePlayerResult> override;
        auto RemovePlayerAsync(service::RemovePlayerParam param) -> Future<service::RemovePlayerResult> override;

        auto RemoveGameInstanceAsync(service::RemoveGameInstanceParam param) -> Future<service::RemoveGameInstanceResult> override;

        auto BroadcastChattingAsync(service::BroadcastChattingParam param) -> Future<service::BroadcastChattingResult> override;

        auto RequestDungeonMatchAsync(service::RequestDungeonMatchParam param) -> Future<service::RequestDungeonMatchResult> override;
        auto CancelDungeonMatchAsync(service::CancelDungeonMatchParam param) -> Future<service::CancelDungeonMatchResult> override;
        auto ApproveDungeonMatchAsync(service::ApproveDungeonMatchParam param) -> Future<service::ApproveDungeonMatchResult> override;
        auto RejectDungeonMatchAsync(service::RejectDungeonMatchParam param) -> Future<service::RejectDungeonMatchResult> override;

    public:
        auto PublishGameInstanceId() -> coordination::game_instance_id_type;

    public:
        auto GetStrand() -> Strand&;
        auto GetServiceLocator() -> ServiceLocator&;

        auto GetNodeContainer() -> coordination::NodeContainer&;
        auto GetLoadBalancer() -> coordination::ILoadBalancer&;
        auto GetChannelInputHandlerFactory() -> const coordination::CommandResponseHandlerFactory&;

    private:
        auto FindGameUser(const std::string& token, coordination::game_instance_id_type instanceId,
            coordination::game_server_id_type serverId) -> coordination::GameUser*;
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
        int64_t _nextGameUserId = 0;

        std::optional<std::pair<std::string, int32_t>> _lobby;
        UniquePtrNotNull<coordination::NodeContainer> _nodeContainer;
        UniquePtrNotNull<coordination::ILoadBalancer> _loadBalancer;
        UniquePtrNotNull<coordination::CommandResponseHandlerFactory> _commandResponseHandlerFactory;
        SharedPtrNotNull<DungeonMatchCoordinator> _dungeonMatchCoordinator;
    };
}
