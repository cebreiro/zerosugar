#pragma once
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"
#include "zerosugar/xr/service/coordination/node/node_id.h"

namespace zerosugar::xr
{
    class CoordinationService;
}

namespace zerosugar::xr::coordination
{
    class GameServer;

    class CommandResponseChannelRunner final
        : public std::enable_shared_from_this<CommandResponseChannelRunner>
    {
    public:
        CommandResponseChannelRunner() = delete;

        CommandResponseChannelRunner(CoordinationService& coordinationService,
            AsyncEnumerable<service::CoordinationCommandResponse> responseEnumerable,
            SharedPtrNotNull<Channel<service::CoordinationCommand>> commandChannel);

        void Start();

        auto GetName() const -> std::string_view;

    private:
        auto Run() -> Future<void>;

    private:
        CoordinationService& _coordinationService;
        AsyncEnumerable<service::CoordinationCommandResponse> _responseEnumerable;
        SharedPtrNotNull<Channel<service::CoordinationCommand>> _commandChannel;
        std::weak_ptr<GameServer> _server;
    };
}
