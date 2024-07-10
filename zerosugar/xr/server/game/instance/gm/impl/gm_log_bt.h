#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class LogBT final : public IGMCommandT<>
    {
    public:
        static constexpr const char* name = "log_bt";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player) override;
    };
}
