#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/login_service_message.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr { class RPCClient; }

namespace zerosugar::xr::service
{
    class ILoginService : public IService
    {
    public:
        static constexpr const char* name = "LoginService";

    public:
        virtual ~ILoginService() = default;

        virtual auto LoginAsync(LoginParam param) -> Future<LoginResult> = 0;
        virtual auto CreateAccountAsync(CreateAccountParam param) -> Future<CreateAccountResult> = 0;
        virtual auto AuthenticateAsync(AuthenticateParam param) -> Future<AuthenticateResult> = 0;
        virtual auto Test1Async(AsyncEnumerable<TestParam> param) -> Future<TestResult> = 0;
        virtual auto Test2Async(TestParam param) -> AsyncEnumerable<TestResult> = 0;
        virtual auto Test3Async(AsyncEnumerable<TestParam> param) -> AsyncEnumerable<TestResult> = 0;
        auto GetName() const -> std::string_view override { return "LoginService"; }
    };

    class LoginServiceProxy final
        : public ILoginService
        , public std::enable_shared_from_this<LoginServiceProxy>
    {
    public:
        explicit LoginServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto LoginAsync(LoginParam param) -> Future<LoginResult> override;
        auto CreateAccountAsync(CreateAccountParam param) -> Future<CreateAccountResult> override;
        auto AuthenticateAsync(AuthenticateParam param) -> Future<AuthenticateResult> override;
        auto Test1Async(AsyncEnumerable<TestParam> param) -> Future<TestResult> override;
        auto Test2Async(TestParam param) -> AsyncEnumerable<TestResult> override;
        auto Test3Async(AsyncEnumerable<TestParam> param) -> AsyncEnumerable<TestResult> override;
    private:
        SharedPtrNotNull<RPCClient> _client;
    };

    void Configure(const SharedPtrNotNull<ILoginService>& service, RPCClient& rpcClient);
}
