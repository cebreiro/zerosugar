#include "login_session_state_machine.h"

#include "zerosugar/shared/execution/executor/impl/asio_strand.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/login_cs_message.h"
#include "zerosugar/xr/server/login/login_session_state.h"

namespace zerosugar::xr
{
    LoginSessionStateMachine::LoginSessionStateMachine(ServiceLocator& serviceLocator, Session& session)
        : _session(session.weak_from_this())
        , _name(fmt::format("login_session_state_machine[{}]", session.GetId()))
        , _serviceLocator(serviceLocator)
        , _channel(std::make_shared<Channel<Buffer>>())
    {
        AddState<login::ConnectedState>(true, *this, serviceLocator, session)
            .Add(LoginSessionState::Authenticated);

        AddState<login::AuthenticatedState>(false, session);
    }

    void LoginSessionStateMachine::Start()
    {
        const std::shared_ptr<Session>& session = _session.lock();
        assert(session);

        Post(session->GetStrand(),
            [](SharedPtrNotNull<LoginSessionStateMachine> self, session::id_type id, WeakPtrNotNull<Session> weak) -> Future<void>
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

    void LoginSessionStateMachine::Shutdown()
    {
        _shutdown.store(true);

        _channel->Close();
    }

    void LoginSessionStateMachine::Receive(Buffer buffer)
    {
        _channel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    auto LoginSessionStateMachine::GetName() const -> std::string_view
    {
        return _name;
    }

    auto LoginSessionStateMachine::Run() -> Future<void>
    {
        [[maybe_unused]]
        auto self = shared_from_this();

        AsyncEnumerable<Buffer> enumerable(_channel);

        Buffer receiveBuffer;

        while (enumerable.HasNext())
        {
            Buffer buffer = co_await enumerable;

            const std::shared_ptr<Session> session = _session.lock();

            if (_shutdown.load() || !session)
            {
                ZEROSUGAR_LOG_WARN(self->_serviceLocator,
                    fmt::format("[{}] fail to find session. io runner exit",
                        self->GetName()));

                co_return;
            }

            receiveBuffer.MergeBack(std::move(buffer));

            while (receiveBuffer.GetSize() >= 4)
            {
                PacketReader reader(receiveBuffer.cbegin(), receiveBuffer.cend());

                const int64_t packetSize = reader.Read<int32_t>();
                if (receiveBuffer.GetSize() < packetSize)
                {
                    break;
                }

                std::unique_ptr<IPacket> packet = network::login::cs::CreateFrom(reader);

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

                    session->Close();

                    co_return;
                }
            }
        }
    }
}
