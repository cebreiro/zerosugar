#include "launch_game_instance_response_handler.h"

#include "zerosugar/xr/service/coordination/coordination_service.h"
#include "zerosugar/xr/service/coordination/node/game_instance.h"
#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"

namespace zerosugar::xr::coordination
{
    auto LaunchGameInstanceResponseHandler::HandleResponse(CoordinationService& service, GameServer& server, const command::response::LaunchGameInstance& response) const
        -> Future<void>
    {
        const auto id = game_instance_id_type(response.gameInstanceId);
        auto gameInstance = std::make_shared<GameInstance>(id, response.zoneId);

        const bool inserted = service.GetNodeContainer().Add(gameInstance);
        if (inserted)
        {
            [[maybe_unused]]
            const bool added = server.AddChild(id, gameInstance.get());
            assert(added);

            gameInstance->SetParent(&server);

            server.MarkCommandResponseAsSuccess(response.responseId);
        }
        else
        {
            assert(false);

            ZEROSUGAR_LOG_CRITICAL(service.GetServiceLocator(),
                std::format("fail to add game_instance to NodeContainer"));

            try
            {
                throw std::runtime_error(std::format("[{}] invalid response", __FUNCTION__));
            }
            catch (...)
            {
                server.MarkCommandResponseAsFailure(response.responseId, std::current_exception());
            }
        }

        co_return;
    }
}
