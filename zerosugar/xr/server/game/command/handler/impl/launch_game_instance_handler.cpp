#include "launch_game_instance_handler.h"

namespace zerosugar::xr
{
    auto LaunchGameInstanceHandler::HandleCommand(GameServer& server, const coordination::command::LaunchGameInstance& command) const -> Future<void>
    {
        const int32_t zoneId = command.zoneId;
        const int64_t instanceId = command.gameInstanceId;
    }
}
