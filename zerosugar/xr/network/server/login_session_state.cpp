#include "login_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/model/generated/login_cs_generated.h"
#include "zerosugar/xr/service/model/generated/login_service_generated_interface.h"

namespace zerosugar::xr
{
    ConnectedState::ConnectedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::Connected)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session)
    {
    }

    auto ConnectedState::OnEvent(const IPacket& iPacket) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(_session.GetStrand()));

        using namespace service;
        using namespace network::login::cs;

        switch (iPacket.GetOpcode())
        {
        case Login::opcode:
        {
            const Login* packet = iPacket.Cast<Login>();
            assert(packet);

            ILoginService& service = _serviceLocator.Get<ILoginService>();

            LoginParam param{
                .account = packet->account,
                .password = packet->password,
            };

            const LoginResult& result = co_await service.LoginAsync(std::move(param));
            assert(ExecutionContext::IsEqualTo(_session.GetStrand()));

            if (result.errorCode == LoginServiceErrorCode::LoginErrorNone)
            {
                
            }
            else
            {
                
            }
        }
        break;
        case CreateAccount::opcode:
        {
            const CreateAccount* packet = iPacket.Cast<CreateAccount>();
            assert(packet);

            
        }
        break;
        default:;
        }

        throw std::runtime_error(std::format("unhandled packet. opcode: {}", iPacket.GetOpcode()));
    }

    AuthenticatedState::AuthenticatedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::Authenticated)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session)
    {
    }

    auto AuthenticatedState::OnEvent(const IPacket& iPacket) -> Future<void>
    {
        co_return;
    }

    TransitionToLobbyState::TransitionToLobbyState(Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::TransitionToLobby)
        , _session(session)
    {
    }

    void TransitionToLobbyState::OnEnter()
    {
        execution::IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        Delay(std::chrono::seconds(3))
            .Then(*executor, [weak = _session.weak_from_this()]()
                {
                    const std::shared_ptr<Session>& session = weak.lock();
                    if (session)
                    {
                        session->Close();
                    }
                });
    }

    auto TransitionToLobbyState::OnEvent(const IPacket& iPacket) -> Future<void>
    {
        (void)iPacket;

        co_return;
    }
}
