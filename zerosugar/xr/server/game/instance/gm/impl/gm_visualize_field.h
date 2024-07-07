#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class VisualizeField final : public IGMCommandT<>
    {
    public:
        static constexpr const char* name = "visualize_field";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player) override;
    };
}
