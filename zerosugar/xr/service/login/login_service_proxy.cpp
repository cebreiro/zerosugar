#include "login_service_proxy.h"

#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/service/model/generated/login_service_generated_json_serialize.h"

namespace zerosugar::xr
{
    LoginServiceProxy::LoginServiceProxy(SharedPtrNotNull<ServiceClient> client)
        : _client(std::move(client))
    {
    }

    auto LoginServiceProxy::LoginAsync(service::LoginParam param) -> Future<service::LoginResult>
    {
        const auto result = co_await _client->CallRemoteProcedure<service::LoginParam, service::LoginResult>("LoginAsync", param);

        if (result.has_value())
        {
            co_return *result;
        }
        else
        {
            throw std::runtime_error(std::format("rpc error. error_code: {}", GetEnumName(result.error())));
        }
    }

    auto LoginServiceProxy::CreateAccountAsync(service::CreateAccountParam param)
        ->Future<service::CreateAccountResult>
    {
        const auto result = co_await _client->CallRemoteProcedure<service::CreateAccountParam, service::CreateAccountResult>("CreateAccountAsync", param);

        if (result.has_value())
        {
            co_return *result;
        }
        else
        {
            throw std::runtime_error(std::format("rpc error. error_code: {}", GetEnumName(result.error())));
        }
    }
}
