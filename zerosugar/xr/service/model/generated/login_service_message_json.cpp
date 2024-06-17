#include "login_service_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, LoginParam& item)
    {
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
    }

    void to_json(nlohmann::json& j, const LoginParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
                { "password", item.password },
            };
    }

    void from_json(const nlohmann::json& j, LoginResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const LoginResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "authenticationToken", item.authenticationToken },
            };
    }

    void from_json(const nlohmann::json& j, CreateAccountParam& item)
    {
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
    }

    void to_json(nlohmann::json& j, const CreateAccountParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
                { "password", item.password },
            };
    }

    void from_json(const nlohmann::json& j, CreateAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const CreateAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, AuthenticateParam& item)
    {
        j.at("token").get_to(item.token);
    }

    void to_json(nlohmann::json& j, const AuthenticateParam& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
            };
    }

    void from_json(const nlohmann::json& j, AuthenticateResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("accountId").get_to(item.accountId);
    }

    void to_json(nlohmann::json& j, const AuthenticateResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "accountId", item.accountId },
            };
    }

    void from_json(const nlohmann::json& j, TestParam& item)
    {
        j.at("token").get_to(item.token);
    }

    void to_json(nlohmann::json& j, const TestParam& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
            };
    }

    void from_json(const nlohmann::json& j, TestResult& item)
    {
        j.at("token").get_to(item.token);
    }

    void to_json(nlohmann::json& j, const TestResult& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
            };
    }

}
