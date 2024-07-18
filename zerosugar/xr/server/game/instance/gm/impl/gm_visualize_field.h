#pragma once
#include "zerosugar/xr/server/game/instance/gm/gm_command_interface.h"

namespace zerosugar::xr
{
    class NavigationService;
}

namespace zerosugar::xr::gm
{
    class VisualizeField final : public IGMCommandT<>
    {
    public:
        static constexpr const char* name = "vis";

    private:
        bool HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player) override;

        static void RegisterObserver(GameExecutionSerial& serialContext, const SharedPtrNotNull<NavigationService>& navi, const std::string& key);
    };
}
