#include "game_cs_message_json.h"

namespace zerosugar::xr::network::game::cs
{
    void from_json(const nlohmann::json& j, Authenticate& item)
    {
        j.at("authenticationToken").get_to(item.authenticationToken);
    }

    void to_json(nlohmann::json& j, const Authenticate& item)
    {
        j = nlohmann::json
            {
                { "authenticationToken", item.authenticationToken },
            };
    }

}
