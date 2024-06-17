
#include "login_service.h"

#include "login_service_message_json.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"

namespace zerosugar::xr::service
{
    LoginServiceProxy::LoginServiceProxy(SharedPtrNotNull<RPCClient> client)
        : _client(std::move(client))
    {
    }

    auto LoginServiceProxy::LoginAsync(LoginParam param) -> Future<LoginResult>
    {
        return _client->CallRemoteProcedure<LoginParam, Future<LoginResult>::value_type>(name, "LoginAsync", std::move(param));
    }

    auto LoginServiceProxy::CreateAccountAsync(CreateAccountParam param) -> Future<CreateAccountResult>
    {
        return _client->CallRemoteProcedure<CreateAccountParam, Future<CreateAccountResult>::value_type>(name, "CreateAccountAsync", std::move(param));
    }

    auto LoginServiceProxy::AuthenticateAsync(AuthenticateParam param) -> Future<AuthenticateResult>
    {
        return _client->CallRemoteProcedure<AuthenticateParam, Future<AuthenticateResult>::value_type>(name, "AuthenticateAsync", std::move(param));
    }

    auto LoginServiceProxy::Test1Async(AsyncEnumerable<TestParam> param) -> Future<TestResult>
    {
        return _client->CallRemoteProcedureClientStreaming<AsyncEnumerable<TestParam>::value_type, Future<TestResult>::value_type>(name, "Test1Async", std::move(param));
    }

    auto LoginServiceProxy::Test2Async(TestParam param) -> AsyncEnumerable<TestResult>
    {
        return _client->CallRemoteProcedureServerStreaming<TestParam, AsyncEnumerable<TestResult>::value_type>(name, "Test2Async", std::move(param));
    }

    auto LoginServiceProxy::Test3Async(AsyncEnumerable<TestParam> param) -> AsyncEnumerable<TestResult>
    {
        return _client->CallRemoteProcedureClientServerStreaming<AsyncEnumerable<TestParam>::value_type, AsyncEnumerable<TestResult>::value_type>(name, "Test3Async", std::move(param));
    }

    void Configure(const SharedPtrNotNull<ILoginService>& service, RPCClient& rpcClient)
    {
        rpcClient.RegisterProcedure<false, false>("LoginService", "LoginAsync",
            [service = service](LoginParam param) -> Future<LoginResult>
            {
                return service->LoginAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("LoginService", "CreateAccountAsync",
            [service = service](CreateAccountParam param) -> Future<CreateAccountResult>
            {
                return service->CreateAccountAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<false, false>("LoginService", "AuthenticateAsync",
            [service = service](AuthenticateParam param) -> Future<AuthenticateResult>
            {
                return service->AuthenticateAsync(std::move(param));
            });
        rpcClient.RegisterProcedure<true, false>("LoginService", "Test1Async",
            [service = service](AsyncEnumerable<TestParam> param) -> Future<TestResult>
            {
                return service->Test1Async(std::move(param));
            });
        rpcClient.RegisterProcedure<false, true>("LoginService", "Test2Async",
            [service = service](TestParam param) -> AsyncEnumerable<TestResult>
            {
                return service->Test2Async(std::move(param));
            });
        rpcClient.RegisterProcedure<true, true>("LoginService", "Test3Async",
            [service = service](AsyncEnumerable<TestParam> param) -> AsyncEnumerable<TestResult>
            {
                return service->Test3Async(std::move(param));
            });
    }

}
