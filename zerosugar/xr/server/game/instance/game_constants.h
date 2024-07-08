#pragma once
#include "zerosugar/xr/server/game/instance/game_type.h"

namespace zerosugar::xr::game_constant
{
    constexpr auto null_time_point = game_time_point_type::min();

    constexpr int32_t inventory_size = 30;
    constexpr int32_t inventory_null_slot = -1;
    constexpr int32_t max_item_overlap_quantity = 200;

    // CharacterData.csv
    constexpr float player_height = 90.f;
    constexpr float player_radius = 45.f;
}
