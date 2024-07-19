#pragma once
#include <boost/container/small_vector.hpp>
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    class PlayerAttackEffectApply final : public GameTaskBaseParamT<IPacket, network::game::cs::ApplyPlayerAttack, MainTargetSelector, PlayerAttackEffectTargetSelector>
    {
    public:
        PlayerAttackEffectApply(UniquePtrNotNull<network::game::cs::ApplyPlayerAttack> param, game_entity_id_type playerId,
            std::span<const game_entity_id_type> targetIds,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type, PlayerAttackEffectTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        boost::container::small_vector<std::pair<game_entity_id_type, float>, 8> _aliveMonsters;

        struct DeadContext
        {
            game_entity_id_type entityId;
            game_controller_id_type controllerId;
            double animationDuration = 0.0;
            std::optional<game_entity_id_type> spawnerId = std::nullopt;
        };
        boost::container::small_vector<DeadContext, 8> _deadMonsters;
    };
}
