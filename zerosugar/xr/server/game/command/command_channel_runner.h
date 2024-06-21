#pragma once
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"

namespace zerosugar::xr
{
    class GameServer;

    class CommandChannelRunner final
        : public std::enable_shared_from_this<CommandChannelRunner>
    {
    public:
        CommandChannelRunner() = delete;

        CommandChannelRunner(GameServer& server, SharedPtrNotNull<execution::IExecutor> executor,
            AsyncEnumerable<service::CoordinationCommand> commandEnumerable);

        void Start();

        auto GetName() const -> std::string_view;

    private:
        auto Run() -> Future<void>;

    private:
        GameServer& _server;
        SharedPtrNotNull<Strand> _strand;

        AsyncEnumerable<service::CoordinationCommand> _commandEnumerable;
    };
}
