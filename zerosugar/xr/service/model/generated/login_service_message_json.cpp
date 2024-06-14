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

}
