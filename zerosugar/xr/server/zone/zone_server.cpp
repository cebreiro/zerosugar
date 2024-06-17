#include "zone_server.h"

namespace zerosugar::xr
{
    ZoneServer::ZoneServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
    {
    }

    void ZoneServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);
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

    auto ZoneServer::GetName() -> std::string_view
    {
        return "zone_server";
    }
}
