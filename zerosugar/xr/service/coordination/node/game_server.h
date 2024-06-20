#pragma once
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/service/coordination/node/node.h"
#include "zerosugar/xr/service/coordination/node/node_id.h"
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"
#include "zerosugar/xr/service/model/generated/coordination_command_message_json.h"

namespace zerosugar::xr::coordination
{
    class GameInstance;

    class GameServer
        : public Node<void, GameInstance*, game_instance_id_type>
        , public std::enable_shared_from_this<GameServer>
    {
    public:
        GameServer(game_server_id_type id, std::string name, std::string ip, uint16_t port);

        void Shutdown();

        auto FindGameInstance(int32_t zoneId) -> GameInstance*;

        void MarkCommandResponseAsSuccess(int64_t responseId);
        void MarkCommandResponseAsFailure(int64_t responseId, const std::exception_ptr& exception);

        auto GetId() const -> const game_server_id_type&;
        auto GetName() const -> std::string_view;
        auto GetIP() const -> std::string_view;
        auto GetPort() const -> uint16_t;

        void SetChannel(SharedPtrNotNull<Channel<service::CoordinationCommand>> channel);

        template <typename T> requires std::derived_from<T, IPacket>
        void SendCommand(const T& command);

        template <typename T> requires std::derived_from<T, IPacket>
        void SendCommand(const T& command, Future<void>& responseCompletionToken);

    private:
        void Send(const service::CoordinationCommand& output);
        void Send(const service::CoordinationCommand& output, Future<void>& responseCompletionToken);

    private:
        bool _shutdown = false;

        game_server_id_type _id = {};
        std::string _name;
        std::string _ip;
        uint16_t _port = 0;
        SharedPtrNotNull<Channel<service::CoordinationCommand>> _channel;

        int64_t _nextCommandResponseId = 0;
        std::unordered_map<int64_t, Promise<void>> _responseWaits;
    };

    template <typename T> requires std::derived_from<T, IPacket>
    void GameServer::SendCommand(const T& command)
    {
        if (_shutdown)
        {
            return;
        }

        service::CoordinationCommand item;
        item.opcode = T::opcode;
        item.contents = nlohmann::json(command).dump();

        this->Send(item);
    }

    template <typename T> requires std::derived_from<T, IPacket>
    void GameServer::SendCommand(const T& command, Future<void>& responseCompletionToken)
    {
        if (_shutdown)
        {
            return;
        }

        service::CoordinationCommand item;
        item.responseId = ++_nextCommandResponseId;
        item.opcode = T::opcode;
        item.contents = nlohmann::json(command).dump();

        this->Send(item, responseCompletionToken);;
    }
}
