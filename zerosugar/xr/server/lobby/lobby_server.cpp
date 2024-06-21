#include "lobby_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/lobby/lobby_session_state.h"
#include "zerosugar/xr/service/model/generated/coordination_service.h"
#include "zerosugar/xr/service/model/generated/gateway_service.h"

namespace zerosugar::xr
{
    LobbyServer::LobbyServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
    {
    }

    void LobbyServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void LobbyServer::StartUp(uint16_t listenPort)
    {
        Server::StartUp(listenPort);

        execution::IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        service::ICoordinationService& coordinationService = _serviceLocator.Get<service::ICoordinationService>();

        service::RequestSnowflakeKeyParam requestSnowflakeKeyParam;
        requestSnowflakeKeyParam.requester = GetName();

        const service::RequestSnowflakeKeyResult res = coordinationService.RequestSnowflakeKeyAsync(std::move(requestSnowflakeKeyParam)).Get();
        if (res.errorCode != service::CoordinationServiceErrorCode::CoordinationErrorNone)
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                std::format("[{}] fail to get snowflake key: {}", GetName(), GetEnumName(res.errorCode)));

            return;
        }

        service::AddGameServiceParam param;
        param.address.name = "default";
        param.address.ip = _ip;
        param.address.port = listenPort;

        service::AddGameServiceResult addResult = _serviceLocator.Get<service::IGatewayService>().AddGameServiceAsync(std::move(param)).Get();
        if (addResult.errorCode != service::GatewayServiceErrorCode::GatewayErrorNone)
        {
            ZEROSUGAR_LOG_CRITICAL(_serviceLocator,
                std::format("[{}] fail to add game service. erorr: {}", GetName(), GetEnumName(addResult.errorCode)));
        }

        _snowflake.emplace(res.snowflakeKey);
    }

    void LobbyServer::SetPublicIP(const std::string& ip)
    {
        _ip = ip;
    }

    void LobbyServer::OnAccept(Session& session)
    {
        assert(_snowflake.has_value());

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[{}] accept session. session: {}", GetName(), session));

        auto stateMachine = std::make_shared<LobbyServerSessionStateMachine>(_serviceLocator, *_snowflake, session);

        {
            decltype(_stateMachines)::accessor accessor;

            if (_stateMachines.insert(accessor, session.GetId()))
            {
                accessor->second = stateMachine;
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        stateMachine->Start();
    }

    void LobbyServer::OnReceive(Session& session, Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        LobbyServerSessionStateMachine* stateMachine = nullptr;
        {
            decltype(_stateMachines)::accessor accessor;

            if (_stateMachines.find(accessor, session.GetId()))
            {
                stateMachine = accessor->second.get();
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        stateMachine->Receive(std::move(buffer));
    }

    void LobbyServer::OnError(Session& session, const boost::system::error_code& error)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[{}] session io error. session: {}, error: {}", GetName(), session, error.message()));

        std::shared_ptr<LobbyServerSessionStateMachine> stateMachine;
        {
            decltype(_stateMachines)::accessor accessor;

            if (_stateMachines.find(accessor, session.GetId()))
            {
                stateMachine = std::move(accessor->second);
            }

            _stateMachines.erase(accessor);
        }

        if (stateMachine)
        {
            stateMachine->Shutdown();
        }
    }

    auto LobbyServer::GetName() -> std::string_view
    {
        return "lobby_server";
    }
}
