#include "ai_controller.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/shared/ai/behavior_tree/log/behavior_tree_log_service_adapter.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"
#include "zerosugar/xr/server/game/instance/ai/aggro/aggro_container.h"
#include "zerosugar/xr/server/game/instance/ai/movement/movement_controller.h"

namespace zerosugar::xr
{
    AIController::AIController(GameInstance& gameInstance, const bt::NodeSerializer& nodeSerializer,
        game_controller_id_type id, game_entity_id_type entityId, std::string btName)
        : _gameInstance(gameInstance)
        , _nodeSerializer(nodeSerializer)
        , _id(id)
        , _entityId(entityId)
        , _behaviorTreeName(std::move(btName))
        , _blackBoard(std::make_unique<bt::BlackBoard>())
        , _mt(std::random_device{}())
        , _aggroContainer(std::make_unique<ai::AggroContainer>(*this))
        , _movementController(std::make_unique<ai::MovementController>(*this))
    {
        _blackBoard->Insert<AIController*>(name, this);

        _prevBehaviorTreeStack.reserve(max_prev_behavior_tree_stack_size);
    }

    AIController::~AIController()
    {
    }

    bool AIController::IsRunning() const
    {
        return !_shutdown;
    }

    void AIController::Start()
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        Post(_gameInstance.GetStrand(), [self = shared_from_this()]()
            {
                self->Transition(self->_behaviorTreeName);
            });
    }

    void AIController::Shutdown()
    {
        assert(_behaviorTree);
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (_shutdown)
        {
            return;
        }

        ++_eventCounter;

        _shutdown = true;
        _behaviorTree->RequestStop();
    }

    auto AIController::Join() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));
        assert(_shutdown);

        if (_runAI.IsValid())
        {
            co_await _runAI;
        }

        _runAI = Future<void>();

        co_return;
    }

    auto AIController::Transition(const std::string& behaviorTreeName) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        [[maybe_unused]]
        auto self = shared_from_this();

        const GameDataProvider& gameDataProvider = _gameInstance.GetServiceLocator().Get<GameDataProvider>();

        const bt::INodeDataSet* dataSet = gameDataProvider.FindBehaviorTree(behaviorTreeName);
        if (!dataSet)
        {
            assert(false);

            co_return;
        }

        ++_eventCounter;

        auto newBehaviorTree = std::make_shared<BehaviorTree>(*_blackBoard);
        newBehaviorTree->Initialize(behaviorTreeName, dataSet->Deserialize(_nodeSerializer));
        newBehaviorTree->SetLogger(_behaviorTreeLogger.get());

        if (_behaviorTree)
        {
            _behaviorTree->RequestStop();

            if (std::ssize(_prevBehaviorTreeStack) == max_prev_behavior_tree_stack_size)
            {
                assert(!_prevBehaviorTreeStack.empty());

                _prevBehaviorTreeStack.erase(_prevBehaviorTreeStack.begin());
            }

            _prevBehaviorTreeStack.push_back(std::move(_behaviorTree));
        }

        _behaviorTree = std::move(newBehaviorTree);

        if (_runAI.IsValid())
        {
            co_await _runAI;
        }

        _runAI = RunAI();
    }

    auto AIController::ReturnPrevBehaviorTree() -> Future<bool>
    {
        if (_prevBehaviorTreeStack.empty())
        {
            co_return false;
        }

        ++_eventCounter;

        SharedPtrNotNull<BehaviorTree> prevBehaviorTree = std::move(_prevBehaviorTreeStack.back());
        _prevBehaviorTreeStack.pop_back();

        assert(_behaviorTree);

        _behaviorTree->RequestStop();
        _behaviorTree.reset();

        _behaviorTree = std::move(prevBehaviorTree);

        if (_runAI.IsValid())
        {
            co_await _runAI;
        }

        _runAI = RunAI();

        co_return true;
    }

    bool AIController::HasDifferenceEventCounter(int64_t counter) const
    {
        return _eventCounter != counter;
    }

    auto AIController::PublishEventCounter() -> int64_t
    {
        return ++_eventCounter;
    }

    void AIController::Notify(const IPacket& packet)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (const auto* message = packet.Cast<network::game::sc::BeAttackedMonster>();
            message && game_entity_id_type::FromInt64(message->attackedId) == GetEntityId())
        {
            _aggroContainer->Add(game_entity_id_type::FromInt64(message->attackerId), 100);
        }

        if (_behaviorTree && _behaviorTree->IsWaitFor(network::game::sc::GetPacketTypeInfo(packet.GetOpcode())))
        {
            assert(_behaviorTree->IsAwaiting());

            network::game::sc::Visit(packet, [this]<typename T>(const T & item)
            {
                _behaviorTree->Notify(item);
            });

            assert(_behaviorTree->CanResume());
        }
    }

    void AIController::InvokeOnBehaviorTree(const std::function<void(BehaviorTree&)>& function)
    {
        ++_eventCounter;

        assert(function);

        function(*_behaviorTree);
    }

    auto AIController::GetControllerId() const -> game_controller_id_type
    {
        return _id;
    }

    void AIController::SetControllerId(game_controller_id_type id)
    {
        assert(false);

        _id = id;
    }

    auto AIController::GetId() const -> game_controller_id_type
    {
        return _id;
    }

    auto AIController::GetEntityId() const -> game_entity_id_type
    {
        return _entityId;
    }

    auto AIController::GetGameInstance() -> GameInstance&
    {
        return _gameInstance;
    }

    auto AIController::GetGameInstance() const -> const GameInstance&
    {
        return _gameInstance;
    }

    auto AIController::GetBlackBoard() -> bt::BlackBoard&
    {
        return *_blackBoard;
    }

    auto AIController::GetRandomEngine() -> std::mt19937&
    {
        return _mt;
    }

    auto AIController::GetAggroContainer() -> ai::AggroContainer&
    {
        return *_aggroContainer;
    }

    auto AIController::GetAggroContainer() const -> const ai::AggroContainer&
    {
        return *_aggroContainer;
    }

    auto AIController::GetMovementController() -> ai::MovementController&
    {
        return *_movementController;
    }

    auto AIController::GetMovementController() const -> const ai::MovementController&
    {
        return *_movementController;
    }

    void AIController::SetBehaviorTreeLogger(std::shared_ptr<IBehaviorTreeLogger> logger)
    {
        _behaviorTreeLogger = std::move(logger);

        _behaviorTree->SetLogger(_behaviorTreeLogger.get());
    }

    auto AIController::RunAI() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        [[maybe_unused]]
        auto self = shared_from_this();

        const SharedPtrNotNull<BehaviorTree> bt = _behaviorTree;
        auto completionToken = std::make_shared<future::SharedContext<void>>();

        while (true)
        {
            constexpr auto tickInterval = std::chrono::milliseconds(200);

            co_await Delay(tickInterval);
            assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

            if (bt->StopRequested())
            {
                break;
            }

            bt->RunOnce();

            while (bt->IsAwaiting())
            {
                completionToken->Reset();

                bt->SetSignalHandler([completionToken]()
                    {
                        completionToken->OnSuccess();
                    });

                co_await Future<void>(completionToken);

                if (bt->StopRequested())
                {
                    break;
                }

                bt->Resume();
            }
        }

        bt->Finalize();

        co_return;
    }
}
