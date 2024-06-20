#include "command_channel_runner.h"

#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/service/model/generated/coordination_command_message.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message_json.h"

namespace zerosugar::xr
{
    CommandChannelRunner::CommandChannelRunner(GameServer& server, SharedPtrNotNull<execution::IExecutor> executor,
        AsyncEnumerable<service::CoordinationCommand> commandEnumerable)
        : _server(server)
        , _strand(std::make_shared<Strand>(std::move(executor)))
        , _commandEnumerable(std::move(commandEnumerable))
    {
    }

    void CommandChannelRunner::Start()
    {
        Post(*_strand, [self = shared_from_this()]()
            {
                self->Run();
            });
    }

    auto CommandChannelRunner::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        using namespace coordination::command;

        response::Authenticate authenticate;
        authenticate.serverId = _server.GetServerId();

        service::CoordinationCommandResponse response;
        response.opcode = response::Authenticate::opcode;
        response.contents = nlohmann::json(authenticate).dump();

        _server.SendCommandResponse(response);

        try
        {
            while (_commandEnumerable.HasNext())
            {
                const service::CoordinationCommand command = co_await _commandEnumerable;

                
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_ERROR(_server.GetServiceLocator(),
                std::format("[channel_runner] command handler throws. exception: {}", e.what()));
        }
    }
}
