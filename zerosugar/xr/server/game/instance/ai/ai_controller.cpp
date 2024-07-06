#include "ai_controller.h"

#include "zerosugar/shared/ai/behavior_tree/behavior_tree.h"
#include "zerosugar/shared/ai/behavior_tree/black_board.h"
#include "zerosugar/shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "zerosugar/xr/data/game_data_provider.h"
#include "zerosugar/xr/data/provider/behavior_tree_xml_provider.h"
#include "zerosugar/xr/network/model/generated/game_sc_message.h"
#include "zerosugar/xr/server/game/instance/game_instance.h"

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
    {
        _blackBoard->Insert<AIController*>("controller", this);
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

        _shutdown = true;
        _behaviorTree->RequestStop();
    }

    auto AIController::Join() -> Future<void>
    {
        assert(_runAI.IsValid());
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        co_await _runAI;

        _runAI = Future<void>();

        co_return;
    }

    auto AIController::Transition(const std::string& behaviorTreeName) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        const GameDataProvider& gameDataProvider = _gameInstance.GetServiceLocator().Get<GameDataProvider>();
        const BehaviorTreeXMLProvider& behaviorTreeProvider = gameDataProvider.GetBehaviorTreeXMLDataProvider();

        const bt::INodeDataSet* dataSet = behaviorTreeProvider.Find(behaviorTreeName);
        if (!dataSet)
        {
            assert(false);

            co_return;
        }

        auto newBehaviorTree = std::make_shared<BehaviorTree>(*_blackBoard);
        newBehaviorTree->Initialize(behaviorTreeName, dataSet->Deserialize(_nodeSerializer));
        newBehaviorTree->SetLogger(_behaviorTreeLogger);

        if (_behaviorTree)
        {
            _behaviorTree->RequestStop();
            _behaviorTree.reset();
        }

        if (_runAI.IsValid())
        {
            co_await _runAI;
        }

        _behaviorTree = std::move(newBehaviorTree);
        _runAI = RunAI();
    }

    bool AIController::IsSubscriberOf(int32_t opcode) const
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));

        if (!_behaviorTree)
        {
            return false;
        }

        if (_behaviorTree->IsAwaiting())
        {
            return _behaviorTree->IsWaitFor(network::game::sc::GetPacketTypeInfo(opcode));
        }

        return false;
    }

    void AIController::Notify(const IPacket& packet)
    {
        assert(ExecutionContext::IsEqualTo(_gameInstance.GetStrand()));
        assert(_behaviorTree);
        assert(_behaviorTree->IsAwaiting());

        network::game::sc::Visit(packet, [this]<typename T>(const T& item)
        {
            _behaviorTree->Notify(item);
        });

        assert(_behaviorTree->CanResume());
    }

    void AIController::InvokeOnBehaviorTree(const std::function<void(BehaviorTree&)>& function)
    {
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

    void AIController::SetBehaviorTreeLogger(IBehaviorTreeLogger* logger)
    {
        _behaviorTreeLogger = logger;

        _behaviorTree->SetLogger(_behaviorTreeLogger);
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

            if (bt->StopRequested())
            {
                break;
            }
        }

        bt->Finalize();

        co_return;
    }
}
