#include "game_sc_message_json.h"

namespace zerosugar::xr::network::game::sc
{
    void from_json(const nlohmann::json& j, Test& item)
    {
        (void)j;
        (void)item;
    }

    void to_json(nlohmann::json& j, const Test& item)
    {
        (void)j;
        (void)item;
    }

}
