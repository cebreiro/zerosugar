#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class ItemAdd final : public IGMCommandT<int32_t, std::optional<int32_t>>
    {
    public:
        static constexpr const char* name = "item_add";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const int32_t& itemId, const std::optional<int32_t>& quantity) override;
    };
}
