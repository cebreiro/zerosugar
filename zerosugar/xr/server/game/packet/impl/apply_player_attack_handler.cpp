#include "apply_player_attack_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"

namespace zerosugar::xr
{
    using network::game::cs::ApplyPlayerAttack;

    auto ApplyPlayerAttackHandler::HandlePacket(GameServer& server, Session& session,
        UniquePtrNotNull<ApplyPlayerAttack> packet) -> Future<void>
    {
        SharedPtrNotNull<GameClient> client = server.FindClient(session.GetId());
        if (!client)
        {
            session.Close();

            co_return;
        }

        const std::shared_ptr<GameInstance> instance = client->GetGameInstance();
        if (!instance)
        {
            session.Close();

            co_return;
        }

        (void)client;
        (void)instance;
    }
}
