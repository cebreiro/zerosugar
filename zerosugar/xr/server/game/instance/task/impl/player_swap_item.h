#pragma once
#include <boost/container/small_vector.hpp>
#include "zerosugar/xr/data/enum/equip_position.h"
#include "zerosugar/xr/network/model/generated/game_cs_message.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"
#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_change_log.h"
#include "zerosugar/xr/server/game/instance/contents/inventory/inventory_item.h"

namespace zerosugar::xr::game_task
{
    class PlayerSwapItem final : public GameTaskBaseParamT<IPacket, network::game::cs::SwapItem, MainTargetSelector>
    {
    public:
        PlayerSwapItem(UniquePtrNotNull<IPacket> param, game_entity_id_type targetId,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MainTargetSelector::target_type target) override;
        void OnComplete(GameExecutionSerial& serialContext) override;

    private:
        game_entity_id_type _id;

        std::optional<data::EquipPosition> _equipPosition = {};
        std::optional<InventoryItem> _newEquipment;

        network::game::sc::NotifySwapItemResult _sync;
    };
}
