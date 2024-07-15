#include "login_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"

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
        TransitionGuard(LoginSessionStateMachine& stateMachine, LoginSessionState state)
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
        LoginSessionStateMachine& _stateMachine;
        LoginSessionState _state = {};
    };
}

namespace zerosugar::xr::login
{
    ConnectedState::ConnectedState(LoginSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LoginSessionStateMachine::state_type(LoginSessionState::Connected)
        , _stateMachine(stateMachine)
        , _serviceLocator(serviceLocator)
        , _session(session.weak_from_this())
    {
        assert(_serviceLocator.Contains<service::ILoginService>());
    }

    auto ConnectedState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        using namespace service;
        using namespace network::login;

        [[maybe_unused]]
        auto stateMachine = _stateMachine.shared_from_this();

        std::shared_ptr<Session> session = _session.lock();
        if (!session)
        {
            co_return;
        }

        assert(ExecutionContext::IsEqualTo(session->GetStrand()));

        std::optional<TransitionGuard> transitionGuard = std::nullopt;

        switch (inPacket->GetOpcode())
        {
        case cs::Login::opcode:
        {
            const cs::Login* packet = inPacket->Cast<cs::Login>();
            assert(packet);

            ILoginService& service = _serviceLocator.Get<ILoginService>();

            LoginParam loginParam{
                .account = packet->account,
                .password = packet->password,
            };

            const LoginResult& serviceResult = co_await service.LoginAsync(std::move(loginParam));
            assert(ExecutionContext::IsEqualTo(session->GetStrand()));

            sc::LoginResult result;
            result.success = serviceResult.errorCode == LoginServiceErrorCode::LoginErrorNone;

            if (result.success)
            {
                IGatewayService& gatewayService = _serviceLocator.Get<IGatewayService>();

                const GetGameServiceListResult& getListResult = co_await gatewayService.GetGameServiceListAsync(GetGameServiceListParam{});
                assert(ExecutionContext::IsEqualTo(session->GetStrand()));

                if (!getListResult.addresses.empty())
                {
                    result.authenticationToken = serviceResult.authenticationToken;
                    result.lobbyIp = getListResult.addresses.front().ip;
                    result.lobbyPort = getListResult.addresses.front().port;

                    transitionGuard.emplace(_stateMachine, LoginSessionState::Authenticated);
                }
                else
                {
                    assert(false);
                }
            }

            session->Send(Packet::ToBuffer(result));
        }
        break;
        case cs::CreateAccount::opcode:
        {
            const cs::CreateAccount* packet = inPacket->Cast<cs::CreateAccount>();
            assert(packet);

            ILoginService& service = _serviceLocator.Get<ILoginService>();

            CreateAccountParam serviceParam{
                .account = packet->account,
                .password = packet->password,
            };

            const CreateAccountResult& serviceResult = co_await service.CreateAccountAsync(std::move(serviceParam));

            assert(ExecutionContext::IsEqualTo(session->GetStrand()));

            sc::CreateAccountResult result;
            result.success = serviceResult.errorCode == LoginServiceErrorCode::LoginErrorNone;

            session->Send(Packet::ToBuffer(result));

            co_return;
        }
        default:;
        }

        if (transitionGuard.has_value())
        {
            co_return;
        }

        throw std::runtime_error(fmt::format("unhandled packet. opcode: {}", inPacket->GetOpcode()));
    }

    AuthenticatedState::AuthenticatedState(Session& session)
        : LoginSessionStateMachine::state_type(LoginSessionState::Authenticated)
        , _session(session.weak_from_this())
    {
    }

    void AuthenticatedState::OnEnter()
    {
        execution::IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        Delay(std::chrono::seconds(3))
            .Then(*executor, [weak = _session]()
                {
                    const std::shared_ptr<Session>& session = weak.lock();
                    if (session)
                    {
                        session->Close();
                    }
                });
    }

    auto AuthenticatedState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        (void)inPacket;

        co_return;
    }
}
