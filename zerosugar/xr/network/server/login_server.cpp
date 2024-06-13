#include "login_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/xr/network/packet_reader.h"
#include "zerosugar/xr/network/model/generated/login_cs_generated.h"

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
                accessor->second.second = stateMachine;
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        stateMachine->Start();

        ZEROSUGAR_LOG_INFO(_serviceLocator,
            std::format("[{}] accept session. session: {}", GetName(), session));
    }

    void LoginServer::OnReceive(Session& session, Buffer buffer)
    {
        Buffer* receivedBuffer = nullptr;
        LoginServerSessionStateMachine* stateMachine = nullptr;
        {
            decltype(_stateMachines)::accessor accessor;

            if (_stateMachines.find(accessor, session.GetId()))
            {
                auto& pair = accessor->second;

                receivedBuffer = &pair.first;
                stateMachine = pair.second.get();
            }
            else
            {
                assert(false);

                session.Close();

                return;
            }
        }

        receivedBuffer->MergeBack(std::move(buffer));

        if (receivedBuffer->GetSize() < 2)
        {
            return;
        }

        PacketReader reader(receivedBuffer->cbegin(), receivedBuffer->cend());

        const int64_t packetSize = reader.Read<int16_t>();
        if (receivedBuffer->GetSize() < packetSize)
        {
            return;
        }

        std::unique_ptr<IPacket> packet = network::login::cs::CreateFrom(reader);

        Buffer temp;
        [[maybe_unused]] bool sliced = receivedBuffer->SliceFront(temp, packetSize);
        assert(sliced);

        if (!packet)
        {
            ZEROSUGAR_LOG_WARN(_serviceLocator,
                std::format("[{}] unnkown packet. session: {}", GetName(), session));

            session.Close();

            return;
        }

        Future<void> future = stateMachine->OnEvent(std::move(packet));

        Post(GetExecutor(),
            [](Future<void> future, SharedPtrNotNull<LoginServer> self, WeakPtrNotNull<Session> weak) mutable -> Future<void>
            {
                try
                {
                    co_await future;
                }
                catch (const std::exception& e)
                {
                    std::shared_ptr<Session> session = weak.lock();
                    if (!session)
                    {
                        co_return;
                    }

                    session->Close();

                    ZEROSUGAR_LOG_WARN(self->_serviceLocator, std::format("[{}] packet handler throws. session: {}, exsception: {}",
                        self->GetName(), *session, e.what()));
                }

            }, std::move(future), SharedFromThis(), session.weak_from_this());
    }

    void LoginServer::OnError(Session& session, const boost::system::error_code& error)
    {
        std::shared_ptr<LoginServerSessionStateMachine> stateMachine;
        {
            decltype(_stateMachines)::accessor accessor;

            if (_stateMachines.find(accessor, session.GetId()))
            {
                stateMachine = std::move(accessor->second.second);
            }

            _stateMachines.erase(accessor);
        }

        if (stateMachine)
        {
            stateMachine->Shutdown();
        }

        ZEROSUGAR_LOG_INFO(_serviceLocator,
            std::format("[{}] session io error. session: {}, error: {}", GetName(), session, error.message()));
    }

    auto LoginServer::GetName() -> std::string_view
    {
        return "login_server";
    }

    auto LoginServer::SharedFromThis() -> SharedPtrNotNull<LoginServer>
    {
        auto result = std::static_pointer_cast<LoginServer>(shared_from_this());

        assert(std::dynamic_pointer_cast<LoginServer>(shared_from_this()) == result);

        return result;
    }
}
