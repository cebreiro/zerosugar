#include "lobby_session_state_machine.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/lobby_cs_message.h"
#include "zerosugar/xr/server/lobby/lobby_session_state.h"

namespace zerosugar::xr
{
    LobbySessionStateMachine::state_type::state_type(LobbySessionState state)
        : StateMachine::state_type(state)
    {
    }

    LobbySessionStateMachine::LobbySessionStateMachine(ServiceLocator& serviceLocator, IUniqueIDGenerator& idGenerator, Session& session)
        : _session(session.weak_from_this())
        , _name(fmt::format("lobby_session_state_machine[{}]", session.GetId()))
        , _serviceLocator(serviceLocator)
        , _channel(std::make_shared<Channel<Buffer>>())
    {
        AddState<lobby::ConnectedState>(true, *this, serviceLocator, session)
            .Add(LobbySessionState::Authenticated);

        AddState<lobby::AuthenticatedState>(false, *this, serviceLocator, idGenerator, session)
            .Add(LobbySessionState::TransitionToGame);

        AddState<lobby::TransitionToGameState>(false, session);
    }

    void LobbySessionStateMachine::Start()
    {
        const std::shared_ptr<Session>& session = _session.lock();
        assert(session);

        Post(session->GetStrand(),
            [](SharedPtrNotNull<LobbySessionStateMachine> self, session::id_type id, WeakPtrNotNull<Session> weak) -> Future<void>
            {
                try
                {
                    co_await self->Run();
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_DEBUG(self->_serviceLocator,
                        fmt::format("[{}] exit with exception. session_id: {}, exception: {}", self->GetName(), id, e.what()));
                }

                if (const std::shared_ptr<Session>& session = weak.lock())
                {
                    session->Close();
                }

            }, shared_from_this(), session->GetId(), _session);
    }

    void LobbySessionStateMachine::Shutdown()
    {
        _shutdown.store(true);

        _channel->Close();
    }

    void LobbySessionStateMachine::Receive(Buffer buffer)
    {
        _channel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    void LobbySessionStateMachine::HandleSessionClose()
    {
        const std::shared_ptr<Session> session = _session.lock();
        if (!session)
        {
            assert(false);

            return;
        }

        Dispatch(session->GetStrand(),
            [self = shared_from_this()]()
            {
                auto current = self->GetCurrentState();

                auto casted = static_cast<state_type*>(current);
                assert(dynamic_cast<state_type*>(current) == casted);

                if (!casted)
                {
                    assert(false);

                    return;
                }

                casted->OnSessionClose();
            });
    }

    auto LobbySessionStateMachine::GetName() const -> std::string_view
    {
        return _name;
    }

    auto LobbySessionStateMachine::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        AsyncEnumerable<Buffer> enumerable(_channel);

        Buffer receiveBuffer;

        while (enumerable.HasNext())
        {
            Buffer buffer = co_await enumerable;

            std::shared_ptr<Session> session = _session.lock();

            if (_shutdown.load() || !session)
            {
                ZEROSUGAR_LOG_WARN(self->_serviceLocator,
                    fmt::format("[{}] fail to find session. io runner exit",
                        self->GetName()));

                co_return;
            }

            assert(ExecutionContext::IsEqualTo(session->GetStrand()));

            receiveBuffer.MergeBack(std::move(buffer));

            while (receiveBuffer.GetSize() >= 4)
            {
                PacketReader reader(receiveBuffer.cbegin(), receiveBuffer.cend());

                const int64_t packetSize = reader.Read<int32_t>();
                if (receiveBuffer.GetSize() < packetSize)
                {
                    break;
                }

                std::unique_ptr<IPacket> packet = network::lobby::cs::CreateFrom(reader);

                Buffer temp;
                [[maybe_unused]] bool sliced = receiveBuffer.SliceFront(temp, packetSize);
                assert(sliced);

                if (!packet)
                {
                    ZEROSUGAR_LOG_WARN(_serviceLocator,
                        fmt::format("[{}] unnkown packet. session: {}", GetName(), *session));

                    session->Close();

                    co_return;
                }

                try
                {
                    co_await OnEvent(std::move(packet));
                }
                catch (const std::exception& e)
                {
                    ZEROSUGAR_LOG_WARN(self->_serviceLocator,
                        fmt::format("[{}] throws. session: {}, exsception: {}",
                            self->GetName(), *session, e.what()));
                }
            }
        }
    }

    auto LobbySessionStateMachine::GetAccountId() const -> int64_t
    {
        return _accountId;
    }

    auto LobbySessionStateMachine::GetAuthenticationToken() const -> const std::string&
    {
        return _authenticationToken;
    }

    void LobbySessionStateMachine::SetAuthenticationToken(std::string token)
    {
        _authenticationToken = std::move(token);
    }

    void LobbySessionStateMachine::SetAccountId(int64_t accountId)
    {
        _accountId = accountId;
    }
}
