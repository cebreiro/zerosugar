#pragma once
#include <cstdint>
#include "zerosugar/xr/service/model/generated/login_service_generated.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::xr::service
{
    class ILoginService : public IService
    {
    public:
        static constexpr std::string name = "LoginService";

    public:
        virtual ~ILoginService() = default;

        virtual auto LoginAsync(LoginParam param) -> Future<LoginResult> = 0;
        virtual auto CreateAccountAsync(CreateAccountParam param) -> Future<CreateAccountResult> = 0;
    };
}
