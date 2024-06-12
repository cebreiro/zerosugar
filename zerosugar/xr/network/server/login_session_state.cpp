#include "login_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_builder.h"
#include "zerosugar/xr/network/model/generated/login_cs_generated.h"
#include "zerosugar/xr/network/model/generated/login_sc_generated.h"
#include "zerosugar/xr/service/model/generated/login_service_generated_interface.h"

namespace zerosugar::xr
{
    class TransitionGuard
    {
    public:
        TransitionGuard() = delete;
        TransitionGuard(const TransitionGuard&) = delete;
        TransitionGuard& operator=(const TransitionGuard&) = delete;
        TransitionGuard(TransitionGuard&&) noexcept = delete;
        TransitionGuard& operator=(TransitionGuard&&) noexcept = delete;

    public:
        TransitionGuard(LoginServerSessionStateMachine& stateMachine, LoginSessionState state)
            : _stateMachine(stateMachine)
            , _state(state)
        {
        }

        ~TransitionGuard()
        {
            [[maybe_unused]] bool transition = _stateMachine.Transition(_state);
            assert(transition);
        }

    private:
        LoginServerSessionStateMachine& _stateMachine;
        LoginSessionState _state = {};
    };

    LoginServerSessionStateMachine::LoginServerSessionStateMachine(ServiceLocator& serviceLocator, Session& session)
    {
        AddState<ConnectedState>(LoginSessionState::Connected, true, *this, serviceLocator, session)
            .Add(LoginSessionState::Authenticated);

        AddState<AuthenticatedState>(LoginSessionState::Authenticated, false, *this, serviceLocator, session)
            .Add(LoginSessionState::TransitionToLobby);

        AddState<TransitionToLobbyState>(LoginSessionState::TransitionToLobby, false, session);
    }

    ConnectedState::ConnectedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::Connected)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session.weak_from_this())
    {
    }

    auto ConnectedState::OnEvent(const IPacket& inPacket) -> Future<void>
    {
        using namespace service;
        using namespace network::login;

        std::optional<TransitionGuard> transitionGuard = std::nullopt;

        assert(ExecutionContext::IsEqualTo(_session.lock()->GetStrand()));

        switch (inPacket.GetOpcode())
        {
        case cs::Login::opcode:
        {
            const cs::Login* packet = inPacket.Cast<cs::Login>();
            assert(packet);

            ILoginService& service = _serviceLocator.Get<ILoginService>();

            LoginParam serviceParam{
                .account = packet->account,
                .password = packet->password,
            };

            const LoginResult& serviceResult = co_await service.LoginAsync(std::move(serviceParam));

            std::shared_ptr<Session> session = _session.lock();
            if (!session)
            {
                co_return;
            }

            assert(ExecutionContext::IsEqualTo(session->GetStrand()));

            sc::LoginResult result;
            result.errorCode = static_cast<int32_t>(serviceResult.errorCode);

            if (serviceResult.errorCode == LoginServiceErrorCode::LoginErrorNone)
            {
                result.authenticationToken = serviceResult.authenticationToken;

                // TODO: remove hardcoding
                result.lobbyIp = "127.0.0.1";
                result.lobbyPort = 8182;

                transitionGuard.emplace(_stateMachine, LoginSessionState::Authenticated);
            }

            session->Send(PacketBuilder::MakePacket(result));
        }
        break;
        case cs::CreateAccount::opcode:
        {
            const cs::CreateAccount* packet = inPacket.Cast<cs::CreateAccount>();
            assert(packet);

            ILoginService& service = _serviceLocator.Get<ILoginService>();

            CreateAccountParam serviceParam{
                .account = packet->account,
                .password = packet->password,
            };

            const CreateAccountResult& serviceResult = co_await service.CreateAccountAsync(std::move(serviceParam));

            std::shared_ptr<Session> session = _session.lock();
            if (!session)
            {
                co_return;
            }

            assert(ExecutionContext::IsEqualTo(session->GetStrand()));

            sc::CreateAccountResult result;
            result.success = serviceResult.errorCode == LoginServiceErrorCode::LoginErrorNone;

            session->Send(PacketBuilder::MakePacket(result));

            co_return;
        }
        default:;
        }

        if (transitionGuard.has_value())
        {
            co_return;
        }

        throw std::runtime_error(std::format("unhandled packet. opcode: {}", inPacket.GetOpcode()));
    }

    AuthenticatedState::AuthenticatedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::Authenticated)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session.weak_from_this())
    {
    }

    auto AuthenticatedState::OnEvent(const IPacket& inPacket) -> Future<void>
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

    auto TransitionToLobbyState::OnEvent(const IPacket& inPacket) -> Future<void>
    {
        (void)inPacket;

        co_return;
    }
}
