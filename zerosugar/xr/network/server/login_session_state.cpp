#include "login_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_builder.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/network/model/generated/login_sc_message.h"
#include "zerosugar/xr/service/model/generated/login_service.h"

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
        : _session(session)
        , _locator(serviceLocator)
        , _channel(std::make_shared<packet_serial_process_channel_type>())
    {
        AddState<ConnectedState>(LoginSessionState::Connected, true, *this, serviceLocator, _session)
            .Add(LoginSessionState::Authenticated);

        AddState<AuthenticatedState>(LoginSessionState::Authenticated, false, _session);
    }

    void LoginServerSessionStateMachine::Start()
    {
        Post(_session.GetStrand(),
            [](SharedPtrNotNull<LoginServerSessionStateMachine> self, session::id_type id, WeakPtrNotNull<Session> weak) -> Future<void>
            {
                try
                {
                    co_await self->Run();
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_DEBUG(self->_locator,
                        std::format("[login_server_session_state_machine] session_id: {}, exception. e: {}", id, e.what()));
                }

                if (const std::shared_ptr<Session>& session = weak.lock())
                {
                    session->Close();
                }

            }, shared_from_this(), _session.GetId(), _session.weak_from_this());
    }

    void LoginServerSessionStateMachine::Shutdown()
    {
        _shutdown.store(true);

        _channel->Close();
    }

    auto LoginServerSessionStateMachine::OnEvent(std::unique_ptr<IPacket> event) -> Future<void>
    {
        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        _channel->Send(std::make_pair(std::move(promise), std::move(event)), channel::ChannelSignal::NotifyOne);

        return future;
    }

    auto LoginServerSessionStateMachine::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        AsyncEnumerable<decltype(_channel)::element_type::value_type> enumerable(_channel);

        while (enumerable.HasNext())
        {
            auto [promise, packet] = co_await enumerable;

            if (_shutdown.load())
            {
                co_return;
            }

            try
            {
                co_await StateMachine::OnEvent(std::move(packet));

                promise.Set();
            }
            catch (...)
            {
                promise.SetException(std::current_exception());
            }
        }
    }

    ConnectedState::ConnectedState(LoginServerSessionStateMachine& stateMachine, ServiceLocator& serviceLocator, Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::Connected)
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

            LoginParam serviceParam{
                .account = packet->account,
                .password = packet->password,
            };

            const LoginResult& serviceResult = co_await service.LoginAsync(std::move(serviceParam));

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

            session->Send(PacketBuilder::MakePacket(result));

            co_return;
        }
        default:;
        }

        if (transitionGuard.has_value())
        {
            co_return;
        }

        throw std::runtime_error(std::format("unhandled packet. opcode: {}", inPacket->GetOpcode()));
    }

    AuthenticatedState::AuthenticatedState(Session& session)
        : LoginServerSessionStateMachine::state_type(LoginSessionState::Authenticated)
        , _session(session)
    {
    }

    void AuthenticatedState::OnEnter()
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

    auto AuthenticatedState::OnEvent(UniquePtrNotNull<IPacket> inPacket) -> Future<void>
    {
        (void)inPacket;

        co_return;
    }
}
