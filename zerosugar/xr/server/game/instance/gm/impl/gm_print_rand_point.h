#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class PrintRandomPoint final : public IGMCommandT<std::optional<int32_t>>
    {
    public:
        static constexpr const char* name = "print_random_point";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const std::optional<int>&) override;
    };
}
