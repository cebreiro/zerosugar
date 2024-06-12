#pragma once
#include "zerosugar/shared/service/service_interface.h"
#include "zerosugar/shared/network/server/server.h"
#include "zerosugar/xr/network/model/generated/rpc_generated.h"

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

        void Initialize(ServiceLocator& dependencyLocator) override;
        void Shutdown() override;
        void Join(std::vector<boost::system::error_code>& errors) override;

        void StartUp(uint16_t port);

    public:
        using request_handler_type = std::function<Future<network::RemoteProcedureCallErrorCode>(const network::RequestRemoteProcedureCall&)>;
        using result_handler_type = std::function<Future<void>(const network::ResultRemoteProcedureCall&)>;

        void SetRequestHandler(const request_handler_type& handler);
        void SetResultHandler(const result_handler_type& handler);

    private:
        void HandleAccept(Session& session);
        void HandleReceive(Session& session, Buffer buffer);

        auto FindSession(const std::string& serviceName) const -> std::shared_ptr<Session>;

        void SendTo(session::id_type id, Buffer buffer);

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<ServerImpl> _server;

        request_handler_type _requestHandler;
        result_handler_type _resultHandler;

        using service_name_type = std::string;
        std::mutex _serviceSessionIdMutex;
        std::unordered_multimap<session::id_type, service_name_type> _sessionServiceLists;
        tbb::concurrent_hash_map<service_name_type, SharedPtrNotNull<Session>> _sessions;

        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<Buffer>> _receiveBuffers;
    };
}
