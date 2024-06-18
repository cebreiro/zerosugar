#include "game_server.h"

namespace zerosugar::xr::coordination
{
    GameServer::GameServer(game_server_id_type id, std::string name, std::string ip, uint16_t port)
        : _id(id)
        , _name(std::move(name))
        , _ip(std::move(ip))
        , _port(port)
    {
    }

    auto GameServer::GetId() const -> const game_server_id_type&
    {
        return _id;
    }

    auto GameServer::GetName() const -> std::string_view
    {
        return _name;
    }

    auto GameServer::GetIP() const -> std::string_view
    {
        return _ip;
    }

    auto GameServer::GetPort() const -> uint16_t
    {
        return _port;
    }

    void GameServer::Send(const service::CoordinationChannelOutput& output)
    {
        if (_channel->IsOpen())
        {
            _channel->Send(output, channel::ChannelSignal::NotifyOne);
        }
    }
}
