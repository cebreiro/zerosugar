#include "launch_game_instance_handler.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message.h"

namespace zerosugar::xr
{
    using coordination::command::LaunchGameInstance;

    auto LaunchGameInstanceHandler::HandleCommand(GameServer& server,
        LaunchGameInstance command, std::optional<int64_t> responseId) const -> Future<void>
    {
        assert(responseId.has_value());

        const int32_t zoneId = command.zoneId;
        const int64_t instanceId = command.gameInstanceId;

        auto instance = std::make_shared<GameInstance>(
            server.GetGameExecutor().SharedFromThis(),
            server.GetServiceLocator(),
            game_instance_id_type(instanceId),
            zoneId);

        GameInstanceContainer& container = server.GetGameInstanceContainer();

        [[maybe_unused]]
        bool added = container.Add(instance);
        assert(added);

        coordination::command::response::LaunchGameInstance response;
        response.gameInstanceId = instanceId;
        response.zoneId = zoneId;

        server.SendCommandResponse(*responseId, response);

        instance->Start();

        co_return;
    }
}
