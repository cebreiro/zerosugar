#include "time_elapsed_after_movement.h"

#include <pugixml.hpp>
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/xr/server/game/instance/ai/ai_controller.h"
#include "zerosugar/xr/server/game/instance/ai/movement/movement_controller.h"

namespace zerosugar::xr::ai
{
    auto TimeElapsedAfterMovement::Run() -> bt::node::Result
    {
        bt::BlackBoard& blackBoard = GetBlackBoard();
        AIController& controller = *blackBoard.Get<AIController*>(AIController::name);

        const MovementController& movementController = controller.GetMovementController();

        if (movementController.IsMoving())
        {
            return false;
        }

        std::chrono::milliseconds randTime;

        if (std::chrono::milliseconds* stored = blackBoard.GetIf<std::chrono::milliseconds>(name))
        {
            randTime = *stored;
        }
        else
        {
            randTime = SelectRandomTime(blackBoard.Get<AIController*>(AIController::name)->GetRandomEngine());

            blackBoard.Insert(name, randTime);
        }

        auto now = game_clock_type::now();

        if (now > movementController.GetLastMovementEndTimePoint() + randTime)
        {
            blackBoard.Remove(name);

            return true;
        }

        return false;
    }

    auto TimeElapsedAfterMovement::GetName() const -> std::string_view
    {
        return name;
    }

    auto TimeElapsedAfterMovement::SelectRandomTime(std::mt19937& randomEngine) const -> std::chrono::milliseconds
    {
        std::uniform_real_distribution<double> dist(-_rand, _rand);

        return GetMilliFromGameSeconds(std::max(0.0, _time + dist(randomEngine)));
    }

    void from_xml(TimeElapsedAfterMovement& self, const pugi::xml_node& xmlNode)
    {
        if (auto attr = xmlNode.attribute("time"); attr)
        {
            self._time = attr.as_double();
        }
        else
        {
            assert(false);
        }

        if (auto attr = xmlNode.attribute("rand"); attr)
        {
            self._rand = attr.as_double();
        }
        else
        {
            assert(false);
        }
    }
}
