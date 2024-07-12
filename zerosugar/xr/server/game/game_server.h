#pragma once
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/xr/network/packet_interface.h"
#include "zerosugar/xr/service/model/generated/coordination_service_message.h"
#include "zerosugar/xr/service/model/generated/coordination_command_response_message_json.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class GameClient;
    class GameClientContainer;
    class CommandChannelRunner;
    class GameInstanceContainer;
    class IGamePacketHandlerFactory;
    class ICommandHandlerFactory;
    class IGameRepository;
}

namespace zerosugar::xr
{
    class GameServer final : public Server
    {
    public:
        GameServer(execution::AsioExecutor& executor);
        ~GameServer();

        void Initialize(ServiceLocator& serviceLocator) override;

        void StartUp(uint16_t listenPort) override;
        void Shutdown() override;

        bool HasClient(session::id_type id) const;
        bool AddClient(session::id_type id, SharedPtrNotNull<GameClient> client);
        auto FindClient(session::id_type id) const -> SharedPtrNotNull<GameClient>;

        auto ShutdownClient(int64_t userId) -> Future<void>;

        template <typename T> requires std::derived_from<T, IPacket>
        void SendCommandResponse(int64_t responseId, const T& item);
        void SendCommandResponse(const service::CoordinationCommandResponse& response);

        auto GetServerId() const -> int64_t;
        auto GetServiceLocator() -> ServiceLocator&;
        auto GetClientContainer() -> GameClientContainer&;
        auto GetGameInstanceContainer() -> GameInstanceContainer&;
        auto GetCommandHandlerFactory() -> ICommandHandlerFactory&;

        void SetPublicIP(std::string ip);

        auto SharedFromThis() -> SharedPtrNotNull<GameServer>;
        auto SharedFromThis() const -> SharedPtrNotNull<const GameServer>;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        void RegisterToCoordinationService();
        void OpenCoordinationCommandChannel();
        void ScheduleServerStatusReport();

    private:
        ServiceLocator _serviceLocator;

        std::string _ip;
        uint16_t _port = 0;

        int64_t _serverId = 0;
        SharedPtrNotNull<Channel<service::CoordinationCommandResponse>> _responseChannel;
        SharedPtrNotNull<CommandChannelRunner> _channelRunner;
        std::stop_source _serverStatusReportStopSource;

        tbb::concurrent_hash_map<session::id_type, Buffer> _sessionReceiveBuffers;
        UniquePtrNotNull<GameClientContainer> _clientContainer;

        SharedPtrNotNull<IGameRepository> _gameRepository;

        UniquePtrNotNull<GameInstanceContainer> _gameInstanceContainer;
        UniquePtrNotNull<IGamePacketHandlerFactory> _gamePacketHandlerFactory;
        UniquePtrNotNull<ICommandHandlerFactory> _commandHandlerFactory;
    };

    template <typename T> requires std::derived_from<T, IPacket>
    void GameServer::SendCommandResponse(int64_t responseId, const T& item)
    {
        service::CoordinationCommandResponse response;
        response.responseId = responseId;
        response.opcode = T::opcode;
        response.contents = nlohmann::json(item).dump();

        SendCommandResponse(response);
    }
}
