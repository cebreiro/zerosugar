#pragma once
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"
#include "zerosugar/xr/server/game/instance/task/game_task.h"

namespace zerosugar::xr::game_task
{
    struct ItemAdd
    {
        int32_t itemId = 0;
        int32_t quantity = 0;
    };

    struct ItemRemove
    {
        int32_t slot = 0;
    };

    struct ItemChange
    {
        game_entity_id_type playerId;
        std::variant<ItemAdd, ItemRemove> change;
    };

    class PlayerItemChange final : public GameTaskParamT<std::vector<ItemChange>, MultiTargetSelector>
    {
    public:
        explicit PlayerItemChange(std::vector<ItemChange> changes,
            game_time_point_type creationTimePoint = game_clock_type::now());

    private:
        void Execute(GameExecutionParallel& parallelContext, MultiTargetSelector::target_type) override;
        void OnComplete(GameExecutionSerial& serialContext) override;
    };
}
