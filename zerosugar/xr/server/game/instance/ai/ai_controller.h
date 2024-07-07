#pragma once
#include "zerosugar/xr/server/game/instance/controller/game_controller_id.h"
#include "zerosugar/xr/server/game/instance/controller/game_controller_interface.h"
#include "zerosugar/xr/server/game/instance/entity/game_entity_id.h"

namespace zerosugar
{
    class BehaviorTree;
    class IBehaviorTreeLogger;
}

namespace zerosugar::bt
{
    class BlackBoard;
    class NodeSerializer;
}

namespace zerosugar::xr
{
    class GameInstance;
}

namespace zerosugar::xr
{
    class AIController final
        : public IGameController
        , public std::enable_shared_from_this<AIController>
    {
    public:
        AIController(GameInstance& gameInstance, const bt::NodeSerializer& nodeSerializer,
            game_controller_id_type id, game_entity_id_type entityId, std::string btName);
        ~AIController();

        bool IsRunning() const;

        void Start();
        void Shutdown();
        auto Join() -> Future<void>;

        auto Transition(const std::string& behaviorTreeName) -> Future<void>;

        bool HasDifferenceEventCounter(int64_t counter) const;
        auto PublishEventCounter() -> int64_t;
        
        bool IsSubscriberOf(int32_t opcode) const override;
        void Notify(const IPacket& packet) override;

        void InvokeOnBehaviorTree(const std::function<void(BehaviorTree&)>& function);

        auto GetControllerId() const -> game_controller_id_type override;
        void SetControllerId(game_controller_id_type id) override;

        auto GetId() const -> game_controller_id_type;
        auto GetEntityId() const -> game_entity_id_type;
        auto GetGameInstance() -> GameInstance&;
        auto GetGameInstance() const -> const GameInstance&;
        auto GetBlackBoard() -> bt::BlackBoard&;

        void SetBehaviorTreeLogger(IBehaviorTreeLogger* logger);

    private:
        auto RunAI() -> Future<void>;

    private:
        bool _shutdown = false;

        GameInstance& _gameInstance;
        const bt::NodeSerializer& _nodeSerializer;

        game_controller_id_type _id;
        game_entity_id_type _entityId;
        std::string _behaviorTreeName;

        IBehaviorTreeLogger* _behaviorTreeLogger = nullptr;

        int64_t _eventCounter = 0;
        Future<void> _runAI;
        UniquePtrNotNull<bt::BlackBoard> _blackBoard;
        SharedPtrNotNull<BehaviorTree> _behaviorTree;
    };
}
