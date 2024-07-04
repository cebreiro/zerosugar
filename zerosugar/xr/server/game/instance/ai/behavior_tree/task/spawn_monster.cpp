#include "spawn_monster.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/data/provider/map_data.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_spawn.h"

namespace zerosugar::xr::game
{
    auto SpawnMonster::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>("controller");
        const data::MonsterSpawner& param = *blackBoard.Get<PtrNotNull<const data::MonsterSpawner>>("param");

        game_task::MonsterSpawnContext context;
        context.dataId = param.monsterId;
        context.x = param.x;
        context.y = param.y;
        context.z = param.z;

        controller.GetGameInstance().Summit(std::make_unique<game_task::MonsterSpawn>(context), controller.GetId());

        struct Temp{};

        co_await bt::Event<Temp>();

        co_return true;
    }

    auto SpawnMonster::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(SpawnMonster&, const pugi::xml_node&)
    {
    }
}
