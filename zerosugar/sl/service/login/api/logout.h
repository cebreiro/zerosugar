#pragma once
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/sl/service/login/state/auth_token.h"

namespace zerosugar::sl::service::api
{
    class Logout
    {
    public:
        struct Param
        {
            AuthToken authToken = {};
            std::string requester;
        };

        struct Result
        {
            enum class ErrorCode
            {
                None = 0,
                Fail_TokenInvalid,
                Fail_Timeout,
            };

            ErrorCode errorCode = ErrorCode::None;
        };

    public:
        virtual ~Logout() = default;

        [[nodiscard]]
        virtual auto Request(Param param) -> Future<Result> = 0;
    };
}
