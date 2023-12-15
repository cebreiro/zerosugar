#include "login_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/shared/service/service_locator_log.h"
#include "zerosugar/sl/protocol/packet/login/sc/hello.h"
#include "zerosugar/sl/protocol/security/login_packet_decoder.h"
#include "zerosugar/sl/protocol/security/login_packet_encoder.h"
#include "zerosugar/sl/protocol/security/secret_key.h"
#include "zerosugar/sl/server/login/login_client.h"

namespace zerosugar::sl
{
    LoginServer::LoginServer(execution::AsioExecutor& executor, locator_type locator, const ServerConfig& config)
        : Server("sl_login", locator, executor)
        , _locator(std::move(locator))
        , _config(config)
    {
    }

    LoginServer::~LoginServer()
    {
    }

    void LoginServer::StartUp()
    {
        if (!StartUp(PORT))
        {
            throw std::runtime_error(std::format("[{}] fail to start up", GetName()));
        }
    }

    void LoginServer::Shutdown()
    {
        Server::Shutdown();

        _clients.clear();
    }

    auto LoginServer::GetConfig() const -> const ServerConfig&
    {
        return _config;
    }

    void LoginServer::OnAccept(Session& session)
    {
        ZEROSUGAR_LOG_INFO(_locator, std::format("[{}] accept session. session: {}",
            GetName(), session));

        auto client = std::make_shared<LoginClient>(_locator, login_client_id_type(++_nextClientId),
            std::make_shared<Strand>(GetExecutor().SharedFromThis()));
        {
            decltype(_clients)::accessor accessor;
            if (_clients.insert(accessor, session.GetId()))
            {
                accessor->second = client;
            }
            else
            {
                assert(false);

                ZEROSUGAR_LOG_CRITICAL(_locator, std::format("[{}] session id collision. session: {}",
                    GetName(), session));

                session.Close();
                return;
            }
        }

        const uint32_t key1 = PacketSecretKey::LOGIN_KEY_FIRST;
        const uint32_t key2 = PacketSecretKey::LOGIN_KEY_SECOND;

        client->StartPacketProcess(
            std::static_pointer_cast<LoginServer>(shared_from_this()),
            session.shared_from_this(),
            std::make_unique<LoginPacketDecoder>(key1, key2),
            std::make_unique<LoginPacketEncoder>(key1, key2));

        client->SendPacket(login::sc::Hello(key1, key2), false);
    }

    void LoginServer::OnReceive(Session& session, Buffer buffer)
    {
        (void)session;
        (void)buffer;

        decltype(_clients)::const_accessor accessor;
        if (_clients.find(accessor, session.GetId()))
        {
            LoginClient& client = *accessor->second;

            client.ReceiveLoginPacket(std::move(buffer));
        }
        else
        {
            ZEROSUGAR_LOG_WAN(_locator, std::format("[{}] receive buffer but client is not found. session: {}",
                GetName(), session));

            session.Close();
        }
    }

    void LoginServer::OnError(Session& session, const boost::system::error_code& error)
    {
        ZEROSUGAR_LOG_WAN(_locator, std::format("[{}] io error. session: {}, error: {}",
            GetName(), session, error.message()));
    }
}
