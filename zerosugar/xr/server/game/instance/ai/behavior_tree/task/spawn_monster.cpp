#include "spawn_monster.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/data/provider/map_data.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/navigation_event.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_spawn.h"

namespace zerosugar::xr::game
{
    auto SpawnMonster::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>("controller");
        const data::MonsterSpawner& param = *blackBoard.Get<PtrNotNull<const data::MonsterSpawner>>("param");

        NavigationService* naviService = controller.GetGameInstance().GetNavigationService();
        assert(naviService);

        const navi::FVector pos(param.x, param.y, param.z);
        constexpr float radius = 100.f;

        naviService->GetRandomPointAroundCircle(pos, radius)
            .Then(controller.GetGameInstance().GetStrand(),
                [controller = controller.shared_from_this()](std::optional<navi::FVector> result)
                {
                    controller->InvokeOnBehaviorTree([result](BehaviorTree& bt)
                        {
                            const event::NaviCompleteRandomPointAroundCircle event{
                                .point = result,
                            };

                            assert(bt.IsAwaiting());
                            assert(bt.IsWaitFor<event::NaviCompleteRandomPointAroundCircle>());

                            bt.NotifyAndResume(event);
                        });
                });

        const auto va = co_await bt::Event<event::NaviCompleteRandomPointAroundCircle>();
        const std::optional<navi::FVector>& point = std::get<event::NaviCompleteRandomPointAroundCircle>(va).point;

        game_task::MonsterSpawnContext context;
        context.dataId = param.monsterId;

        if (point.has_value())
        {
            context.x = point->GetX();
            context.y = point->GetY();
            context.z = point->GetZ();
        }
        else
        {
            context.x = param.x;
            context.y = param.y;
            context.z = param.z;
        }

        naviService->DrawCircle(navi::FVector(context.x, context.y, context.z), 0.45f);

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
