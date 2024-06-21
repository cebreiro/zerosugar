#include "command_channel_runner.h"

#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/xr/server/game/game_server.h"
#include "zerosugar/xr/server/game/command/handler/command_handler_interface.h"
#include "zerosugar/xr/server/game/command/handler/command_handler_factory_interface.h"
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

        {
            response::Authenticate authenticate;
            authenticate.serverId = _server.GetServerId();

            service::CoordinationCommandResponse response;
            response.opcode = response::Authenticate::opcode;
            response.contents = nlohmann::json(authenticate).dump();

            _server.SendCommandResponse(response);
        }

        while (_commandEnumerable.HasNext())
        {
            try
            {
                const service::CoordinationCommand command = co_await _commandEnumerable;

                const auto& factory = _server.GetCommandHandlerFactory();
                const auto& handler = factory.CreateHandler(command.opcode);

                if (!handler)
                {
                    assert(false);

                    ZEROSUGAR_LOG_CRITICAL(_server.GetServiceLocator(),
                        std::format("[{}] fail to find command handler. opcode: {}", GetName(), command.opcode));

                    continue;
                }

                co_await handler->Handle(_server, command.contents, command.responseId);
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_ERROR(_server.GetServiceLocator(),
                    std::format("[{}] handler throws. exception: {}", GetName(), e.what()));
            }
        }
    }

    auto CommandChannelRunner::GetName() const -> std::string_view
    {
        return "command_channel_runner";
    }
}
