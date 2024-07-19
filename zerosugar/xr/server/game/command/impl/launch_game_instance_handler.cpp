#include "launch_game_instance_handler.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/game_instance_container.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"

namespace zerosugar::xr
{
    using coordination::command::LaunchGameInstance;

    auto ScheduleGameInstanceShutdown(SharedPtrNotNull<GameServer> server, SharedPtrNotNull<GameInstance> instance) -> Future<void>
    {
        co_await instance->GetStrand();

        constexpr auto interval = std::chrono::seconds(10);

        while (true)
        {
            co_await Delay(interval);
            assert(ExecutionContext::IsEqualTo(instance->GetStrand()));

            if (instance->GetSnapshotContainer().GetPlayerCount() > 0)
            {
                continue;
            }

            ZEROSUGAR_LOG_INFO(instance->GetServiceLocator(),
                fmt::format("[game_instance_{}] has no player. it will be shutdown.",
                    instance->GetId()));

            instance->Shutdown();

            [[maybe_unused]]
            const bool removed = server->GetGameInstanceContainer().Remove(instance->GetId());
            assert(removed);

            auto& coordinationService = instance->GetServiceLocator().Get<service::ICoordinationService>();

            service::RemoveGameInstanceParam param;
            param.gameInstanceId = instance->GetId().Unwrap();

            auto removeFuture = coordinationService.RemoveGameInstanceAsync(std::move(param));
            auto joinFuture = instance->Join();

            co_await WaitAll(instance->GetExecutor(), removeFuture, joinFuture);

            service::RemoveGameInstanceResult removeResult = removeFuture.Get();
            if (removeResult.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
            {
                ZEROSUGAR_LOG_CRITICAL(instance->GetServiceLocator(),
                    fmt::format("[game_instance_{}] fail to remove this from coordination service. error: {}",
                        instance->GetId(), GetEnumName(removeResult.errorCode)));
            }

            ZEROSUGAR_LOG_INFO(instance->GetServiceLocator(),
                fmt::format("[game_instance_{}] complete shutdown.",
                    instance->GetId()));

            co_return;
        }
    }

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

        ScheduleGameInstanceShutdown(server.SharedFromThis(), std::move(instance));

        co_return;
    }
}
