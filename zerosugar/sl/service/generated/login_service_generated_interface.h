#pragma once
#include <cstdint>
#include "zerosugar/sl/service/generated/login_service_generated.h"
#include "zerosugar/sl/service/generated/shared_generated.h"
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/shared/execution/channel/channel.h"

namespace zerosugar::sl::service
{
    class ILoginService : public IService
    {
    public:
        virtual ~ILoginService() = default;

        virtual auto LoginAsync(LoginParam param) -> Future<LoginResult> = 0;
        virtual auto LogoutAsync(LogoutParam param) -> Future<LogoutResult> = 0;
        virtual auto AuthenticateAsync(AuthenticateParam param) -> Future<AuthenticateResult> = 0;
        virtual auto AuthorizeAsync(AuthorizeParam param) -> Future<AuthorizeResult> = 0;
        virtual auto KickAsync(KickParam param) -> Future<KickResult> = 0;
    };
}
