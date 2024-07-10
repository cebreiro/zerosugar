#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class MonsterAttack final : public IGMCommandT<std::optional<int32_t>>
    {
    public:
        static constexpr const char* name = "monster_attack";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player, const std::optional<int32_t>& skillId) override;
    };
}
