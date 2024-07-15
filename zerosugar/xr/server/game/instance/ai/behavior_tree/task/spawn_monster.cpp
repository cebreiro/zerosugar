#include "spawn_monster.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/data/table/map.h"
#include "zerosugar/xr/navigation/navigation_service.h"
#include "zerosugar/xr/navigation/navi_vector.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/behavior_tree/event/navigation_event.h"
#include "zerosugar/xr/server/game/instance/task/impl/monster_spawn.h"

namespace zerosugar::xr::ai
{
    auto SpawnMonster::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);
        const data::MonsterSpawner& param = *blackBoard.Get<PtrNotNull<const data::MonsterSpawner>>("param");

        ServiceLocator& serviceLocator = controller.GetGameInstance().GetServiceLocator();

        const MonsterData* data = serviceLocator.Get<GameDataProvider>().Find(monster_data_id_type(param.monsterId));

        if (!data)
        {
            ZEROSUGAR_LOG_ERROR(serviceLocator,
                fmt::format("[{}] fail to find monster data. id: {}", GetName(), param.monsterId));

            co_return false;
        }

        NavigationService* naviService = controller.GetGameInstance().GetNavigationService();
        assert(naviService);

        const navi::FVector pos(param.x, param.y, param.z);
        constexpr float radius = 100.f;

        const int64_t eventCounter = controller.PublishEventCounter();

        naviService->GetRandomPointAroundCircle(pos, radius)
            .Then(controller.GetGameInstance().GetStrand(),
                [weak = controller.weak_from_this(), eventCounter](std::optional<navi::FVector> result)
                {
                    const std::shared_ptr<AIController> controller = weak.lock();
                    if (!controller || controller->HasDifferenceEventCounter(eventCounter))
                    {
                        return;
                    }

                    controller->InvokeOnBehaviorTree([result](BehaviorTree& bt)
                        {
                            assert(bt.IsWaitFor<event::NaviCompleteRandomPointAroundCircle>());

                            const event::NaviCompleteRandomPointAroundCircle event {
                                .point = result,
                            };

                            bt.Notify(event);
                        });
                });

        const auto va = co_await bt::Event<event::NaviCompleteRandomPointAroundCircle>();
        const std::optional<navi::FVector>& point = std::get<event::NaviCompleteRandomPointAroundCircle>(va).point;

        game_task::MonsterSpawnContext context;
        context.data = data;
        context.spawnerId = controller.GetEntityId();

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

        controller.GetGameInstance().Summit(std::make_unique<game_task::MonsterSpawn>(context), controller.GetId());

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
