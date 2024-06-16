#include "login_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"

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
        auto stateMachine = std::make_shared<LoginServerSessionStateMachine>(_serviceLocator, session);

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

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[{}] accept session. session: {}", GetName(), session));
    }

    void LoginServer::OnReceive(Session& session, Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(session.GetStrand()));

        LoginServerSessionStateMachine* stateMachine = nullptr;
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

        std::shared_ptr<LoginServerSessionStateMachine> stateMachine;
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

        ZEROSUGAR_LOG_DEBUG(_serviceLocator,
            std::format("[{}] session io error. session: {}, error: {}", GetName(), session, error.message()));
    }

    auto LoginServer::GetName() -> std::string_view
    {
        return "login_server";
    }
}
