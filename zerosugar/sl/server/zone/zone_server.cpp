#include "zone_server.h"

namespace zerosugar::sl
{
    ZoneServer::ZoneServer(execution::AsioExecutor& executor, int8_t worldId, int32_t zoneId, uint16_t port)
        : Server(std::format("sl_zone_{}", zoneId), executor)
        , _worldId(worldId)
        , _zoneId(zoneId)
        , _port(port)
    {
    }

    ZoneServer::~ZoneServer()
    {
    }

    void ZoneServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _locator = serviceLocator;
    }

    void ZoneServer::StartUp()
    {
        StartUp(_port);
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
