#include "zone_server.h"

namespace zerosugar::sl
{
    ZoneServer::ZoneServer(execution::AsioExecutor& executor, locator_type locator,
        int8_t worldId, int32_t zoneId, uint16_t port)
        : Server(std::format("sl_zone_{}", zoneId), locator, executor)
        , _locator(std::move(locator))
        , _worldId(worldId)
        , _zoneId(zoneId)
        , _port(port)
    {
    }

    ZoneServer::~ZoneServer()
    {
    }

    void ZoneServer::StartUp()
    {
        if (!StartUp(_port))
        {
            throw std::runtime_error(std::format("[{}] fail to start up", GetName()));
        }
    }

    void ZoneServer::Shutdown()
    {
        Server::Shutdown();
    }

    void ZoneServer::OnAccept(Session& session)
    {
        (void)session;
    }

    void ZoneServer::OnReceive(Session& session, Buffer buffer)
    {
        (void)session;
        (void)buffer;
    }

    void ZoneServer::OnError(Session& session, const boost::system::error_code& error)
    {
        (void)session;
        (void)error;
    }
}
