#pragma once
#include "zerosugar/xr/server/game/command/command_handler_interface.h"
#include "zerosugar/xr/service/model/generated/coordination_command_message_json.h"

namespace zerosugar::xr
{
    class BroadcastChattingHandler final : public CommandHandlerT<coordination::command::BroadcastChatting>
    {
    private:
        auto HandleCommand(GameServer& server,
            const coordination::command::BroadcastChatting& command,
            std::optional<int64_t> responseId) const->Future<void> override;
    };
}
