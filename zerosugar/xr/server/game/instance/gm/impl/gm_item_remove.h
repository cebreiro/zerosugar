#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class ItemRemove final : public IGMCommandT<int32_t>
    {
    public:
        static constexpr const char* name = "item_remove";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const int32_t& slot) override;
    };
}
