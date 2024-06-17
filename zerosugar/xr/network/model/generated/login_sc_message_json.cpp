#include "login_sc_message_json.h"

namespace zerosugar::xr::network::login::sc
{
    void from_json(const nlohmann::json& j, CreateAccountResult& item)
    {
        j.at("success").get_to(item.success);
    }

    void to_json(nlohmann::json& j, const CreateAccountResult& item)
    {
        j = nlohmann::json
            {
                { "success", item.success },
            };
    }

    void from_json(const nlohmann::json& j, LoginResult& item)
    {
        j.at("success").get_to(item.success);
        j.at("authenticationToken").get_to(item.authenticationToken);
        j.at("lobbyIp").get_to(item.lobbyIp);
        j.at("lobbyPort").get_to(item.lobbyPort);
    }

    void to_json(nlohmann::json& j, const LoginResult& item)
    {
        j = nlohmann::json
            {
                { "success", item.success },
                { "authenticationToken", item.authenticationToken },
                { "lobbyIp", item.lobbyIp },
                { "lobbyPort", item.lobbyPort },
            };
    }

}
