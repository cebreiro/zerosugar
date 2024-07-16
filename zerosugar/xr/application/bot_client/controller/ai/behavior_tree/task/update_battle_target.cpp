#include "update_battle_target.h"

#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/application/bot_client/controller/bot_controller.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/local_player.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/monster.h"
#include "zerosugar/xr/application/bot_client/controller/ai/vision/visual_object_container.h"
#include "zerosugar/xr/network/model/generated/game_message.h"

namespace zerosugar::xr::bot
{
    auto UpdateBattleTarget::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        BotController& controller = *blackBoard.Get<BotController*>(BotController::name);

        VisualObjectContainer& visualObjectContainer = controller.GetVisualObjectContainer();

        if (std::shared_ptr<Monster>* item = blackBoard.GetIf<std::shared_ptr<Monster>>("battle_target"); item)
        {
            if (visualObjectContainer.Contains((*item)->GetData().id) && (*item)->GetData().hp > 0.f)
            {
                return true;
            }

            blackBoard.Remove("battle_target");
        }

        if (Monster* battleTarget = SelectByRandom(visualObjectContainer, controller.GetRandomEngine()); battleTarget)
        {
            blackBoard.Insert<std::shared_ptr<Monster>>("battle_target", battleTarget->shared_from_this());

            return true;
        }

        return false;
    }

    auto UpdateBattleTarget::GetName() const -> std::string_view
    {
        return name;
    }

    auto UpdateBattleTarget::SelectByMinDistance(VisualObjectContainer& visualObjectContainer, const Eigen::Vector3d& localPos) -> Monster*
    {
        if (!visualObjectContainer.HasMonster())
        {
            return nullptr;
        }

        double minDistanceSq = std::numeric_limits<double>::max();
        Monster* battleTarget = nullptr;

        for (Monster& mob : visualObjectContainer.GetMonsterRange())
        {
            if (mob.GetData().hp <= 0.f)
            {
                continue;
            }

            const double distanceSq = (localPos - mob.GetPosition()).squaredNorm();
            if (distanceSq < minDistanceSq)
            {
                minDistanceSq = distanceSq;
                battleTarget = &mob;
            }
        }

        return battleTarget;
    }

    auto UpdateBattleTarget::SelectByRandom(VisualObjectContainer& visualObjectContainer, std::mt19937& randomEngine) -> Monster*
    {
        if (!visualObjectContainer.HasMonster())
        {
            return nullptr;
        }

        const auto range = visualObjectContainer.GetMonsterRange();
        const int64_t distance = std::ranges::distance(range);
        constexpr int64_t tryCount = 5;

        for (int64_t i = 0; i < tryCount; ++i)
        {
            const auto iter = std::ranges::next(range.begin(), randomEngine() % distance);

            Monster* monster = &(*iter);
            assert(monster);

            if (monster->GetData().hp > 0.f)
            {
                return monster;
            }
        }

        return nullptr;
    }

    void from_xml(UpdateBattleTarget&, const pugi::xml_node&)
    {
    }
}
