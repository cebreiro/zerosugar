#include "player_spawn.h"

#include "zerosugar/xr/data/table/map.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_container.h"
#include "zerosugar/xr/server/game/instance/entity/component/movement_component.h"
#include "zerosugar/xr/server/game/instance/entity/component/stat_component.h"
#include "zerosugar/xr/server/game/instance/task/game_task_scheduler.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_parallel.h"
#include "zerosugar/xr/server/game/instance/task/execution/game_execution_serial.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_controller.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_snapshot_container.h"

namespace zerosugar::xr::game_task
{
    PlayerSpawn::PlayerSpawn(SharedPtrNotNull<GameEntity> player, game_time_point_type creationTimePoint)
        : GameTaskParamT(creationTimePoint, std::move(player), NullSelector{})
    {
    }

    bool PlayerSpawn::ShouldPrepareBeforeScheduled() const
    {
        return true;
    }

    void PlayerSpawn::Prepare(GameExecutionSerial& serialContext, bool& quickExit)
    {
        quickExit = false;

        const game_controller_id_type controllerId = GetParam()->GetController().GetControllerId();
        const game_entity_id_type entityId = GetParam()->GetId();

        serialContext.GetTaskScheduler().AddController(controllerId);
        serialContext.GetTaskScheduler().AddEntity(entityId);
    }

    void PlayerSpawn::Execute(GameExecutionParallel& parallelContext, NullSelector::target_type)
    {
        [[maybe_unused]]
        const bool added = parallelContext.GetEntityContainer().Add(GetParam());
        assert(added);

        ConfigureStat(*GetParam());
    }

    void PlayerSpawn::OnComplete(GameExecutionSerial& serialContext)
    {
        auto player = std::make_unique<GamePlayerSnapshot>(GetParam()->GetController());
        player->Initialize(*GetParam());

        const game_entity_id_type id = player->GetId();

        [[maybe_unused]]
        const bool result = serialContext.GetSnapshotContainer().Add(std::move(player));
        assert(result);

        serialContext.GetSnapshotController().ProcessPlayerSpawn(*GetParam());

        if (serialContext.GetMapData().GetType() == data::MapType::Village)
        {
            serialContext.GetSnapshotController().ProcessPlayerActivate(id);
        }
    }

    void PlayerSpawn::ConfigureStat(GameEntity& entity)
    {
        const auto& inventoryComponent = entity.GetComponent<InventoryComponent>();
        auto& statComponent = entity.GetComponent<StatComponent>();

        for (const InventoryItem& item : inventoryComponent.GetEquippedItemRange())
        {
            if (item.attack)
            {
                statComponent.AddItemStat(StatType::Attack, StatValue(*item.str));
            }

            if (item.defence)
            {
                statComponent.AddItemStat(StatType::Attack, StatValue(*item.defence));
            }

            if (item.str)
            {
                statComponent.AddItemStat(StatType::Str, StatValue(*item.str));
            }

            if (item.dex)
            {
                statComponent.AddItemStat(StatType::Str, StatValue(*item.dex));
            }

            if (item.intell)
            {
                statComponent.AddItemStat(StatType::Str, StatValue(*item.intell));
            }
        }

        const auto initValue = StatValue(100.0);

        statComponent.SetMaxHP(initValue);
        statComponent.SetHP(initValue);
        statComponent.SetMaxMP(initValue);
        statComponent.SetMP(initValue);
        statComponent.SetMaxStamina(initValue);
        statComponent.SetStamina(initValue);
    }
}
