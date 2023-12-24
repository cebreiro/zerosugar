#include "gateway_server.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/network/session/session.h"
#include "zerosugar/sl/protocol/security/gateway_packet_decoder.h"
#include "zerosugar/sl/protocol/security/gateway_packet_encoder.h"
#include "zerosugar/sl/server/gateway/gateway_client.h"

namespace zerosugar::sl
{
    GatewayServer::GatewayServer(execution::AsioExecutor& executor, int8_t worldId)
        : Server("sl_gateway", executor)
        , _worldId(worldId)
        , _publicAddress("127.0.0.1") // TODO: fix
    {
    }

    GatewayServer::~GatewayServer()
    {
    }

    void GatewayServer::Initialize(ServiceLocator& dependencyLocator)
    {
        Server::Initialize(dependencyLocator);

        _locator = dependencyLocator;

        using namespace service;

        auto worldService = _locator.Find<IWorldService>();

        if (!_locator.Contains<ILoginService>() || !worldService)
        {
            throw std::runtime_error(std::format("[{}] service not found", GetName()));
        }

        const CreateWorldParam param{
            .worldId = GetWorldId(),
            .address = GetPublicAddress(),
        };

        const CreateWorldResult& result = worldService->CreateWorldAsync(param).Get();
        if (result.errorCode != WorldServiceErrorCode::WorldErrorNone)
        {
            throw std::runtime_error(std::format("[{}] fail to create world:{}", GetName(), GetWorldId()));
        }
    }

    void GatewayServer::StartUp()
    {
        if (!StartUp(PORT))
        {
            throw std::runtime_error(std::format("[{}] fail to start up", GetName()));
        }
    }

    void GatewayServer::Shutdown()
    {
        Server::Shutdown();
    }

    auto GatewayServer::GetPublicAddress() const -> const std::string&
    {
        return _publicAddress;
    }

    auto GatewayServer::GetWorldId() const -> int8_t
    {
        return _worldId;
    }

    void GatewayServer::OnAccept(Session& session)
    {
        ZEROSUGAR_LOG_INFO(_locator, std::format("[{}] accept session. session: {}",
            GetName(), session));

        auto client = std::make_shared<GatewayClient>(_locator, gateway_client_id_type(++_nextClientId),
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

        client->StartPacketProcess(
            std::static_pointer_cast<GatewayServer>(shared_from_this()),
            session.shared_from_this(),
            std::make_unique<GatewayPacketDecoder>(),
            std::make_unique<GatewayPacketEncoder>()
        );
    }

    void GatewayServer::OnReceive(Session& session, Buffer buffer)
    {
        decltype(_clients)::const_accessor accessor;
        if (_clients.find(accessor, session.GetId()))
        {
            GatewayClient& client = *accessor->second;

            client.ReceiveLoginPacket(std::move(buffer));
        }
        else
        {
            ZEROSUGAR_LOG_WAN(_locator, std::format("[{}] receive buffer but client is not found. session: {}",
                GetName(), session));

            session.Close();
        }
    }

    void GatewayServer::OnError(Session& session, const boost::system::error_code& error)
    {
        ZEROSUGAR_LOG_WAN(_locator, std::format("[{}] io error. session: {}, error: {}",
            GetName(), session, error.message()));
    }
}
