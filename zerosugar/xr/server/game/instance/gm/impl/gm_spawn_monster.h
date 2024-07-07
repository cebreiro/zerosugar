#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr::gm
{
    class SpawnMonster final : public IGMCommandT<int32_t, std::optional<int64_t>>
    {
    public:
        static constexpr const char* name = "spawn_monster";

    private:
        bool HandleCommand(GameExecutionSerial& instance, GamePlayerSnapshot& player,
            const int32_t& mobId, const std::optional<int64_t>& count) override;
    };
}
