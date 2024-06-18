#include "coordination_channel_input_message_json.h"

namespace zerosugar::xr::coordination::input
{
    void from_json(const nlohmann::json& j, Authenticate& item)
    {
        j.at("serverId").get_to(item.serverId);
    }

    void to_json(nlohmann::json& j, const Authenticate& item)
    {
        j = nlohmann::json
            {
                { "serverId", item.serverId },
            };
    }

}
