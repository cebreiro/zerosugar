#pragma once
#include "zerosugar/xr/server/game/command/command_handler_interface.h"
#include "zerosugar/xr/service/model/generated/coordination_command_message_json.h"

namespace zerosugar::xr
{
    class NotifyDungeonMatchGroupCreationHandler final : public CommandHandlerT<coordination::command::NotifyDungeonMatchGroupCreation>
    {
    private:
        auto HandleCommand(GameServer& server,
            coordination::command::NotifyDungeonMatchGroupCreation command,
            std::optional<int64_t> responseId) const -> Future<void> override;
    };
}
