#include "database_service_message_json.h"

namespace zerosugar::xr::service
{
    void from_json(const nlohmann::json& j, AddAccountParam& item)
    {
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
    }

    void to_json(nlohmann::json& j, const AddAccountParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
                { "password", item.password },
            };
    }

    void from_json(const nlohmann::json& j, AddAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
    }

    void to_json(nlohmann::json& j, const AddAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
            };
    }

    void from_json(const nlohmann::json& j, GetAccountParam& item)
    {
        j.at("account").get_to(item.account);
    }

    void to_json(nlohmann::json& j, const GetAccountParam& item)
    {
        j = nlohmann::json
            {
                { "account", item.account },
            };
    }

    void from_json(const nlohmann::json& j, GetAccountResult& item)
    {
        j.at("errorCode").get_to(item.errorCode);
        j.at("accountId").get_to(item.accountId);
        j.at("account").get_to(item.account);
        j.at("password").get_to(item.password);
        j.at("banned").get_to(item.banned);
        j.at("deleted").get_to(item.deleted);
    }

    void to_json(nlohmann::json& j, const GetAccountResult& item)
    {
        j = nlohmann::json
            {
                { "errorCode", item.errorCode },
                { "accountId", item.accountId },
                { "account", item.account },
                { "password", item.password },
                { "banned", item.banned },
                { "deleted", item.deleted },
            };
    }

}
