#pragma once
#include "zerosugar/xr/application/bot_client/controller/bot_session_state.h"

namespace zerosugar
{
    class Socket;
    class BehaviorTree;
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
}

namespace zerosugar::xr
{
    class BotController : public std::enable_shared_from_this<BotController>
    {
    public:
        BotController(const ServiceLocator& locator, SharedPtrNotNull<execution::AsioStrand> strand,
        int64_t id, const bt::NodeSerializer& nodeSerializer, std::string defaultBehaviorTree);
        ~BotController();

        void Start();
        void Shutdown(const std::string& reason);

        auto Transition(const std::string& behaviorTreeName) -> Future<void>;

        auto ConnectTo(std::string ip, uint16_t port, int32_t retryMilli) -> Future<void>;
        auto Close() -> Future<void>;

        void Send(Buffer buffer);

        auto GetId() const -> int64_t;
        auto GetSocket() -> Socket&;
        auto GetSessionState() const -> BotSessionStateType;

        void SetSessionState(BotSessionStateType sessionState);

    private:
        auto RunIO() -> Future<void>;
        auto RunAI() -> Future<void>;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<execution::AsioStrand> _strand;
        int64_t _id;

        const bt::NodeSerializer& _nodeSerializer;
        std::string _defaultBehaviorTree;
        Future<void> _runAI;

        BotSessionStateType _sessionState = BotSessionStateType::Login;
        SharedPtrNotNull<Socket> _socket;
        Future<void> _runIOFuture;

        UniquePtrNotNull<bt::BlackBoard> _blackBoard;
        SharedPtrNotNull<BehaviorTree> _behaviorTree;
    };
}
