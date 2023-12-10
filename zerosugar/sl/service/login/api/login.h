#pragma once
#include <cstdint>
#include <string>
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/sl/service/login/state/auth_token.h"

namespace zerosugar::sl::service::api
{
    class Login
    {
    public:
        struct Param
        {
            std::string account;
            std::string password;
            std::string requester;
        };

        struct Result
        {
            enum class ErrorCode
            {
                None = 0,
                Fail_Invalid,
                Fail_Duplicated,
                Fail_Timeout,
            };

            ErrorCode errorCode = ErrorCode::None;
            AuthToken authToken = {};
            int64_t accountId = 0;
        };

    public:
        virtual ~Login() = default;

        [[nodiscard]]
        virtual auto Request(Param param) -> Future<Result> = 0;
    };
}
