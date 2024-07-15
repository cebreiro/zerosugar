#include "login_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/server/login/login_session_state_machine.h"

namespace zerosugar::xr
{
    LoginServer::LoginServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
    {
    }

    void LoginServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void LoginServer::OnAccept(Session& session)
    {
        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            fmt::format("[{}] accept session. session: {}", GetName(), session));

        auto stateMachine = std::make_shared<LoginSessionStateMachine>(_serviceLocator, session);
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

    void LoginServer::OnReceive(Session& session, Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        LoginSessionStateMachine* stateMachine = nullptr;
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

    void LoginServer::OnError(Session& session, const boost::system::error_code& error)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            fmt::format("[{}] session io error. session: {}, error: {}", GetName(), session, error.message()));

        std::shared_ptr<LoginSessionStateMachine> stateMachine;
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

    auto LoginServer::GetName() -> std::string_view
    {
        return "login_server";
    }
}
