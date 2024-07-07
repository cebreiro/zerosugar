#include "gm_spawn_monster.h"

#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/data/provider/monster_data_provider.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/snapshot/game_player_snapshot.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_spawn.h"

namespace zerosugar::xr::gm
{
    bool SpawnMonster::HandleCommand(GameExecutionSerial& serialContext, GamePlayerSnapshot& player,
        const int32_t& mobId, const std::optional<int64_t>& count)
    {
        const MonsterDataProvider& monsterDataProvider = serialContext.GetServiceLocator().Get<GameDataProvider>().GetMonsterDataProvider();
        const data::Monster* monsterData = monsterDataProvider.Find(mobId);

        if (!monsterData)
        {
            return false;
        }

        const int64_t spawnCount = std::max<int64_t>(1, count.value_or(-1));

        for (int64_t i = 0; i < spawnCount; ++i)
        {
            game_task::MonsterSpawnContext context;
            context.data = monsterData;
            context.x = static_cast<float>(player.GetPosition().x()) + 100.f;
            context.y = static_cast<float>(player.GetPosition().y()) + 100.f;
            context.z = static_cast<float>(player.GetPosition().z());

            auto task = std::make_unique<game_task::MonsterSpawn>(context);

            serialContext.SummitTask(std::move(task), player.GetController().GetControllerId());
        }

        return true;
    }
}
