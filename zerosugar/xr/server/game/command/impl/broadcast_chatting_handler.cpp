#include "broadcast_chatting_handler.h"

#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_global_chat.h"

namespace zerosugar::xr
{
    using coordination::command::BroadcastChatting;

    auto BroadcastChattingHandler::HandleCommand(GameServer& server,
        BroadcastChatting command, [[maybe_unused]] std::optional<int64_t> responseId) const -> Future<void>
    {
        assert(!responseId.has_value());

        server.GetGameInstanceContainer().Visit([message = command.message](GameInstance& instance)
            {
                auto task = std::make_unique<game_task::PlayerGlobalChat>(
                    std::make_pair(ChattingType::Global, message));

                instance.Summit(std::move(task), std::nullopt);
            });

        co_return;
    }
}
