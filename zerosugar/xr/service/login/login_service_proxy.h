#pragma once
#include <boost/asio.hpp>
#include "zerosugar/shared/network/session/event.h"
#include "zerosugar/xr/network/rpc_client.h"
#include "zerosugar/xr/service/model/generated/login_service_generated_interface.h"

namespace zerosugar
{
    class Session;
}

namespace zerosugar::xr
{
    // TODO: code-generation
    class LoginServiceProxy
        : public service::ILoginService
        , public std::enable_shared_from_this<LoginServiceProxy>
    {
    public:
        explicit LoginServiceProxy(SharedPtrNotNull<RPCClient> client);

        auto LoginAsync(service::LoginParam param) -> Future<service::LoginResult> override;
        auto CreateAccountAsync(service::CreateAccountParam param) -> Future<service::CreateAccountResult> override;

    private:
        SharedPtrNotNull<RPCClient> _client;
    };
}
