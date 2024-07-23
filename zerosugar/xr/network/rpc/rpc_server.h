#pragma once
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/xr/network/model/generated/rpc_message.h"

namespace zerosugar::execution
{
    class AsioExecutor;
}

namespace zerosugar::xr
{
    class OrchestratorService;

    class RPCServer final
        : public IService
        , public std::enable_shared_from_this<RPCServer>
    {
        class ServerImpl;

    public:
        RPCServer() = delete;
        explicit RPCServer(SharedPtrNotNull<execution::AsioExecutor> executor);

        void Initialize(ServiceLocator& serviceLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        void StartUp(uint16_t port);

        bool IsOpen() const;

        auto GetName() const -> std::string_view override;

    private:
        void HandleAccept(Session& session);
        void HandleReceive(Session& session, Buffer buffer);

        auto FindSession(const std::string& serviceName) const -> std::shared_ptr<Session>;
        auto FindSession(int64_t rpcId) const -> std::shared_ptr<Session>;

        void SendTo(session::id_type id, Buffer buffer);

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<ServerImpl> _server;
        ServiceLocatorT<ILogService> _serviceLocator;

        using service_name_type = std::string;
        std::mutex _serviceSessionIdMutex;
        std::unordered_multimap<session::id_type, service_name_type> _sessionServiceLists;
        tbb::concurrent_hash_map<service_name_type, SharedPtrNotNull<Session>> _sessions;

        std::atomic<int32_t> _nextSnowFlakeValue = 0;
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<Buffer>> _receiveBuffers;
        tbb::concurrent_hash_map<int64_t, SharedPtrNotNull<Session>> _pendingRPCs;
    };
}
