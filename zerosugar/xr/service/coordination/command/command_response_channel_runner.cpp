#include "command_response_channel_runner.h"

#include "zerosugar/xr/service/coordination/coordination_service.h"
#include "zerosugar/xr/service/coordination/command/command_response_handler_interface.h"
#include "zerosugar/xr/service/coordination/command/command_response_handler_factory.h"
#include "zerosugar/xr/service/coordination/node/game_server.h"
#include "zerosugar/xr/service/coordination/node/node_container.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message_json.h"

namespace zerosugar::xr::coordination
{
    CommandResponseChannelRunner::CommandResponseChannelRunner(CoordinationService& coordinationService,
        AsyncEnumerable<service::CoordinationCommandResponse> responseEnumerable,
        SharedPtrNotNull<Channel<service::CoordinationCommand>> commandChannel)
        : _coordinationService(coordinationService)
        , _responseEnumerable(std::move(responseEnumerable))
        , _commandChannel(std::move(commandChannel))
    {
    }

    void CommandResponseChannelRunner::Start()
    {
        Post(_coordinationService.GetStrand(), [self = shared_from_this()]()
            {
                self->Run();
            });
    }

    auto CommandResponseChannelRunner::GetName() const -> std::string_view
    {
        return "command_response_channel_runner";
    }

    auto CommandResponseChannelRunner::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        try
        {
            using namespace command::response;

            while (_responseEnumerable.HasNext())
            {
                const service::CoordinationCommandResponse response = co_await _responseEnumerable;
                assert(ExecutionContext::IsEqualTo(_coordinationService.GetStrand()));

                switch (response.opcode)
                {
                case Exception::opcode:
                {
                    std::shared_ptr<GameServer> server = _server.lock();
                    if (!server)
                    {
                        assert(false);

                        continue;
                    }

                    const nlohmann::json& json = nlohmann::json::parse(response.contents);
                    const Exception exception = json.get<Exception>();

                    try
                    {
                        throw std::runtime_error(exception.message);
                    }
                    catch (...)
                    {
                        server->MarkCommandResponseAsFailure(response.responseId, std::current_exception());
                    }
                }
                break;
                case Authenticate::opcode:
                {
                    const nlohmann::json& json = nlohmann::json::parse(response.contents);
                    const Authenticate authenticate = json.get<Authenticate>();

                    NodeContainer& container = _coordinationService.GetNodeContainer();

                    GameServer* server = container.Find(game_server_id_type(authenticate.serverId));
                    assert(server);

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
                            fmt::format("[{}] fail to get game server instance", GetName()));

                        co_return;
                    }

                    const auto& factory = _coordinationService.GetChannelInputHandlerFactory();
                    const auto& handler = factory.CreateHandler(response.opcode);

                    if (!handler)
                    {
                        assert(false);

                        ZEROSUGAR_LOG_CRITICAL(_coordinationService.GetServiceLocator(),
                            fmt::format("[{}] fail to find response handler. opcode: {}", GetName(), response.opcode));

                        continue;
                    }

                    try
                    {
                        co_await handler->Handle(_coordinationService, *server, response.contents);

                        server->MarkCommandResponseAsSuccess(response.responseId);
                    }
                    catch (...)
                    {
                        server->MarkCommandResponseAsFailure(response.responseId, std::current_exception());
                    }
                }
                break;
                }
            }
        }
        catch (const std::exception& e)
        {
            ZEROSUGAR_LOG_ERROR(_coordinationService.GetServiceLocator(),
                fmt::format("[{}] handler throws. exception: {}", GetName(), e.what()));
        }
    }
}
