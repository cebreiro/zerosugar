#pragma once
#include "zerosugar/xr/service/coordination/node/node.h"
#include "zerosugar/xr/service/coordination/node/node_id.h"
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"

namespace zerosugar::xr::coordination
{
    class GameInstance;

    class GameServer : public Node<void, GameInstance*, game_instance_id_type>
    {
    public:
        GameServer(game_server_id_type id, std::string name, std::string ip, uint16_t port);

        auto GetId() const -> const game_server_id_type&;
        auto GetName() const -> std::string_view;
        auto GetIP() const -> std::string_view;
        auto GetPort() const -> uint16_t;

        void Send(const service::CoordinationChannelOutput& output);

    private:
        game_server_id_type _id = {};
        std::string _name;
        std::string _ip;
        uint16_t _port = 0;
        SharedPtrNotNull<Channel<service::CoordinationChannelOutput>> _channel;
    };
}
