#include "login_session_state.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_builder.h"
#include "zerosugar/xr/network/packet_reader.h"
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
        : _session(session.weak_from_this())
        , _name(std::format("login_session_state_machine[{}]", session.GetId()))
        , _serviceLocator(serviceLocator)
        , _channel(std::make_shared<Channel<Buffer>>())
    {
        AddState<login::ConnectedState>(true, *this, serviceLocator, session)
            .Add(LoginSessionState::Authenticated);

        AddState<login::AuthenticatedState>(false, session);
    }

    void LoginServerSessionStateMachine::Start()
    {
        const std::shared_ptr<Session>& session = _session.lock();
        assert(session);

        Post(session->GetStrand(),
            [](SharedPtrNotNull<LoginServerSessionStateMachine> self, session::id_type id, WeakPtrNotNull<Session> weak) -> Future<void>
            {
                try
                {
                    co_await self->Run();
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_DEBUG(self->_serviceLocator,
                        std::format("[{}] exit with exception. session_id: {}, exception: {}", self->GetName(), id, e.what()));
                }

                if (const std::shared_ptr<Session>& session = weak.lock())
                {
                    session->Close();
                }

            }, shared_from_this(), session->GetId(), _session);
    }

    void LoginServerSessionStateMachine::Shutdown()
    {
        _shutdown.store(true);

        _channel->Close();
    }

    void LoginServerSessionStateMachine::Receive(Buffer buffer)
    {
        _channel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto LoginServerSessionStateMachine::GetName() const -> std::string_view
    {
        return _name;
    }

    auto LoginServerSessionStateMachine::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        AsyncEnumerable<Buffer> enumerable(_channel);

        Buffer receiveBuffer;

        while (enumerable.HasNext())
        {
            Buffer buffer = co_await enumerable;

            const std::shared_ptr<Session>& session = _session.lock();

            if (_shutdown.load() || !session)
            {
                co_return;
            }

            try
            {
                receiveBuffer.MergeBack(std::move(buffer));

                if (receiveBuffer.GetSize() < 2)
                {
                    continue;
                }

                PacketReader reader(receiveBuffer.cbegin(), receiveBuffer.cend());

                const int64_t packetSize = reader.Read<int16_t>();
                if (receiveBuffer.GetSize() < packetSize)
                {
                    continue;
                }

                std::unique_ptr<IPacket> packet = network::login::cs::CreateFrom(reader);

                Buffer temp;
                [[maybe_unused]] bool sliced = receiveBuffer.SliceFront(temp, packetSize);
                assert(sliced);

                if (!packet)
                {
                    ZEROSUGAR_LOG_WARN(_serviceLocator,
                        std::format("[{}] unnkown packet. session: {}", GetName(), *session));

                    session->Close();

                    co_return;
                }

                co_await OnEvent(std::move(packet));
            }
            catch (const std::exception& e)
            {
                ZEROSUGAR_LOG_WARN(self->_serviceLocator, std::format("[{}] throws. session: {}, exsception: {}",
                    self->GetName(), *session, e.what()));
            }
        }
    }
}

namespace zerosugar::xr::login
{
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
