#include "game_server.h"

namespace zerosugar::xr
{
    GameServer::GameServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
    {
    }

    void GameServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void GameServer::OnAccept(Session& session)
    {
        (void)session;
    }

    void GameServer::OnReceive(Session& session, Buffer buffer)
    {
        (void)session;
        (void)buffer;
    }

    void GameServer::OnError(Session& session, const boost::system::error_code& error)
    {
        (void)session;
        (void)error;
    }

    auto GameServer::GetName() -> std::string_view
    {
        return "zone_server";
    }
}
