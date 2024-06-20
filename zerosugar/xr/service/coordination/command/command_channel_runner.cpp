#include "command_channel_runner.h"

#include "zerosugar/xr/service/coordination/coordination_service.h"
#include "zerosugar/xr/service/coordination/command/handler/command_response_handler_interface.h"
#include "zerosugar/xr/service/coordination/command/handler/command_response_handler_factory.h"
#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message_json.h"

namespace zerosugar::xr::coordination
{
    CommandChannelRunner::CommandChannelRunner(CoordinationService& coordinationService,
        AsyncEnumerable<service::CoordinationCommandResponse> responseEnumerable,
        SharedPtrNotNull<Channel<service::CoordinationCommand>> commandChannel)
        : _coordinationService(coordinationService)
        , _responseEnumerable(std::move(responseEnumerable))
        , _commandChannel(std::move(commandChannel))
    {
    }

    void CommandChannelRunner::Start()
    {
        Post(_coordinationService.GetStrand(), [self = shared_from_this()]()
            {
                self->Run();
            });
    }

    auto CommandChannelRunner::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        try
        {
            using namespace command::response;

            while (_responseEnumerable.HasNext())
            {
                const service::CoordinationCommandResponse input = co_await _responseEnumerable;
                assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

                switch (input.opcode)
                {
                case Authenticate::opcode:
                {
                    const auto authenticate = nlohmann::json(input.contents).get<Authenticate>();

                    NodeContainer& container = _coordinationService.GetNodeContainer();

                    GameServer* server = container.Find(game_server_id_type(authenticate.serverId));
                    server->SetChannel(_commandChannel);

                    _server = server->weak_from_this();
                }
                break;
                default:
                {
                    std::shared_ptr<GameServer> server = _server.lock();
                    if (!server)
                    {
                        ZEROSUGAR_LOG_WARN(_coordinationService.GetServiceLocator(),
                            std::format("[channel_runner] fail to get game server instance"));

                        co_return;
                    }

                    const auto& factory = _coordinationService.GetChannelInputHandlerFactory();
                    const auto& handler = factory.CreateHandler(input.opcode);
                    if (!handler)
                    {
                        assert(false);

                        ZEROSUGAR_LOG_CRITICAL(_coordinationService.GetServiceLocator(),
                            std::format("[channel_runner] fail to find response handler. opcode: {}", input.opcode));

                        continue;
                    }

                    co_await handler->Handle(_coordinationService, *server, input.contents);
                }
                break;
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_ERROR(_coordinationService.GetServiceLocator(),
                std::format("[channel_runner] command response handler throws. exception: {}", e.what()));
        }
    }
}
