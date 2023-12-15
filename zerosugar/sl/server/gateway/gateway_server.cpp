#include "gateway_server.h"

namespace zerosugar::sl
{
    GatewayServer::GatewayServer(execution::AsioExecutor& executor, locator_type locator, const ServerConfig& config)
        : Server("sl_gateway", locator, executor)
        , _locator(std::move(locator))
        , _config(config)
    {
    }

    GatewayServer::~GatewayServer()
    {
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

    void GatewayServer::OnAccept(Session& session)
    {
        (void)session;
    }

    void GatewayServer::OnReceive(Session& session, Buffer buffer)
    {
        (void)session;
        (void)buffer;
    }

    void GatewayServer::OnError(Session& session, const boost::system::error_code& error)
    {
        (void)session;
        (void)error;
    }
}
