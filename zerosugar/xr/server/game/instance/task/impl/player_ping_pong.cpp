#include "player_ping_pong.h"

#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"

namespace zerosugar::xr::game_task
{
    using namespace network::game;

    PlayerPingPong::PlayerPingPong(UniquePtrNotNull<cs::Ping> param, game_entity_id_type targetId,
        game_time_point_type creationTimePoint)
        : GameTaskBaseParamT(creationTimePoint, std::move(param), MainTargetSelector(targetId))
    {
    }

    void PlayerPingPong::Execute(GameExecutionParallel& parallelContext, PtrNotNull<GameEntity> player)
    {
        (void)parallelContext;

        const cs::Ping& ping = GetParam();

        sc::Pong pong;
        pong.sequence = ping.sequence;
        pong.clientTimePoint = ping.clientTimePoint;
        pong.serverTimePoint = GetBaseTime().time_since_epoch().count();

        player->GetController().Notify(pong);
    }

    void PlayerPingPong::OnComplete(GameExecutionSerial& serialContext)
    {
        (void)serialContext;
    }
}
