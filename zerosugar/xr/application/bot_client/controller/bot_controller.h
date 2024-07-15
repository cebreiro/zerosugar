#pragma once
#include "zerosugar/xr/application/bot_client/controller/bot_session_state.h"

namespace zerosugar
{
    class Socket;
    class BehaviorTree;
    class IBehaviorTreeLogger;
}

namespace zerosugar::execution
{
    class AsioStrand;
}

namespace zerosugar::bt
{
    class BlackBoard;
    class NodeSerializer;
}

namespace zerosugar::xr
{
    class IPacket;
    class NavigationService;
}

namespace zerosugar::xr::bot
{
    struct SharedContext;

    class VisualObjectContainer;
    class LocalPlayer;
    class MovementController;
}

namespace zerosugar::xr
{
    class BotController : public std::enable_shared_from_this<BotController>
    {
    public:
        static constexpr const char* name = "bot_controller";

    public:
        BotController(const ServiceLocator& locator, SharedPtrNotNull<Strand> strand, bot::SharedContext& sharedContext,
            SharedPtrNotNull<Socket> socket, int64_t id, const bt::NodeSerializer& nodeSerializer, std::string defaultBehaviorTree);
        ~BotController();

        void Start();
        void StartDebugOutputString();
        void Shutdown(const std::string& reason);

        auto Transition(std::string behaviorTreeName) -> Future<void>;
        void InvokeOnBehaviorTree(const std::function<void(BehaviorTree&)>& func);

        auto ConnectTo(std::string ip, uint16_t port, int32_t retryMilli) -> Future<void>;
        auto Close() -> Future<void>;

        void SendToServer(Buffer buffer);

        auto GetId() const -> int64_t;
        auto GetName() const -> std::string;
        auto GetStrand() const -> Strand&;
        auto GetServiceLocator() -> ServiceLocator&;

        auto GetSocket() -> Socket&;
        auto GetRandomEngine() -> std::mt19937&;
        auto GetSessionState() const -> BotSessionStateType;
        auto GetLocalPlayer() -> bot::LocalPlayer&;
        auto GetLocalPlayer() const -> const bot::LocalPlayer&;
        auto GetVisualObjectContainer() -> bot::VisualObjectContainer&;
        auto GetVisualObjectContainer() const -> const bot::VisualObjectContainer&;
        auto GetMovementController() -> bot::MovementController&;
        auto GetMovementController() const -> const bot::MovementController&;

        auto GetNavigation() -> NavigationService*;

        void SetLogger(IBehaviorTreeLogger* logger);
        void SetSessionState(BotSessionStateType sessionState);

    private:
        auto RunIO() -> Future<void>;
        auto RunAI() -> Future<void>;

        void TryCreateNavigation(int32_t mapId);
        void TryRemoveNavigation(int32_t mapId);

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<Strand> _strand;
        bot::SharedContext& _sharedContext;
        int64_t _id;

        const bt::NodeSerializer& _nodeSerializer;
        IBehaviorTreeLogger* _behaviorTreeLogger = nullptr;

        std::string _defaultBehaviorTree;
        Future<void> _runAI;

        BotSessionStateType _sessionState = BotSessionStateType::Login;
        SharedPtrNotNull<Socket> _socket;
        Future<void> _runIOFuture;
        Buffer _receivedBuffer;
        Buffer _receiveBuffer;

        std::mt19937 _randomEngine;

        UniquePtrNotNull<bt::BlackBoard> _blackBoard;
        SharedPtrNotNull<BehaviorTree> _behaviorTree;

        std::unique_ptr<bot::LocalPlayer> _localPlayer;
        UniquePtrNotNull<bot::VisualObjectContainer> _visualObjectContainer;
        UniquePtrNotNull<bot::MovementController> _movementController;
    };
}
