#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar::bt
{
    class NodeSerializer;
}

namespace zerosugar::xr
{
    class AIController;
    class GameInstance;
}

namespace zerosugar::xr
{
    class AIControlService
    {
    public:
        explicit AIControlService(GameInstance& gameInstance);
        ~AIControlService();

        auto ShutdownAndJoin() -> Future<void>;

        auto CreateAIController(game_entity_id_type entityId, const std::string& btName) -> SharedPtrNotNull<AIController>;
        auto DeleteAIController(game_controller_id_type id) -> Future<void>;

        auto FindAIController(game_entity_id_type entityId) -> AIController*;

    private:
        GameInstance& _gameInstance;

        UniquePtrNotNull<bt::NodeSerializer> _nodeSerializer;

        boost::unordered::unordered_flat_map<
            game_controller_id_type, SharedPtrNotNull<AIController>> _controllers;
        std::unordered_map<game_entity_id_type, AIController*> _aiControllerEntityIndex;
    };
}
