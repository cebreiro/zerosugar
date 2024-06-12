#include "login_service_generated_json_serialize.h"

namespace zerosugar::sl::service
{
    void from_json(const nlohmann::json& j, LoginParam& item)
    {
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
        j.at("address").get_to(item.address);
        j.at("context").get_to(item.context);
    }

    void to_json(nlohmann::json& j, const LoginParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
                { "password", item.password },
                { "address", item.address },
                { "context", item.context },
            };
    }

    void from_json(const nlohmann::json& j, LoginResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("token").get_to(item.token);
        j.at("accountId").get_to(item.accountId);
    }

    void to_json(nlohmann::json& j, const LoginResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "token", item.token },
                { "accountId", item.accountId },
            };
    }

    void from_json(const nlohmann::json& j, LogoutParam& item)
    {
        j.at("token").get_to(item.token);
        j.at("context").get_to(item.context);
    }

    void to_json(nlohmann::json& j, const LogoutParam& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
                { "context", item.context },
            };
    }

    void from_json(const nlohmann::json& j, LogoutResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const LogoutResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, AuthenticateParam& item)
    {
        j.at("token").get_to(item.token);
        j.at("address").get_to(item.address);
    }

    void to_json(nlohmann::json& j, const AuthenticateParam& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
                { "address", item.address },
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

    void from_json(const nlohmann::json& j, AuthorizeParam& item)
    {
        j.at("token").get_to(item.token);
        j.at("type").get_to(item.type);
        j.at("typeValue1").get_to(item.typeValue1);
        j.at("typeValue2").get_to(item.typeValue2);
    }

    void to_json(nlohmann::json& j, const AuthorizeParam& item)
    {
        j = nlohmann::json
            {
                { "token", item.token },
                { "type", item.type },
                { "typeValue1", item.typeValue1 },
                { "typeValue2", item.typeValue2 },
            };
    }

    void from_json(const nlohmann::json& j, AuthorizeResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const AuthorizeResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, KickParam& item)
    {
        j.at("account").get_to(item.account);
        j.at("context").get_to(item.context);
    }

    void to_json(nlohmann::json& j, const KickParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
                { "context", item.context },
            };
    }

    void from_json(const nlohmann::json& j, KickResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const KickResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

}
