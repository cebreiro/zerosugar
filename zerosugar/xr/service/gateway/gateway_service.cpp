#include "gateway_service.h"

#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr
{
    GatewayService::GatewayService(SharedPtrNotNull<execution::IExecutor> executor)
        : _executor(std::move(executor))
    {
    }

    void GatewayService::Initialize(ServiceLocator& serviceLocator)
    {
        Configure(shared_from_this(), serviceLocator.Get<RPCClient>());
    }

    void GatewayService::Shutdown()
    {
    }

    void GatewayService::Join(std::vector<boost::system::error_code>& errors)
    {
        (void)errors;
    }

    auto GatewayService::AddGameServiceAsync(service::AddGameServiceParam param)
        -> Future<service::AddGameServiceResult>
    {
        const std::string& key = param.address.name;
        const std::pair<std::string, int32_t>& value = std::make_pair(param.address.ip, param.address.port);

        const bool inserted = _gameServices.emplace(key, value).second;

        service::AddGameServiceResult result;
        result.errorCode = inserted ? service::GatewayServiceErrorCode::GatewayErrorNone : service::GatewayServiceErrorCode::AddGameErrorNameDuplicated;

        co_return result;
    }

    auto GatewayService::GetGameServiceListAsync(service::GetGameServiceListParam param)
        -> Future<service::GetGameServiceListResult>
    {
        service::GetGameServiceListResult result;
        result.addresses.reserve(_gameServices.size());

        for (const auto& [serviceName, address] : _gameServices)
        {
            service::GameServiceEndpoint endpoint;
            endpoint.name = serviceName;
            endpoint.ip = address.first;
            endpoint.port = address.second;

            result.addresses.emplace_back(std::move(endpoint));
        }

        co_return result;
    }
}
