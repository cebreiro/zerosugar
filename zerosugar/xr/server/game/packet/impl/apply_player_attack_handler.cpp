#include "apply_player_attack_handler.h"

#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/client/game_client.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/task/impl/player_attack_effect_apply.h"

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

        const auto range = packet->targets | std::views::transform([](int64_t value)
            {
                return game_entity_id_type::FromInt64(value);
            }) | std::ranges::to<std::vector>();

        auto task = std::make_unique<game_task::PlayerAttackEffectApply>(std::move(packet),
            client->GetGameEntityId(), range);

        instance->Summit(std::move(task), client->GetControllerId());
    }
}
