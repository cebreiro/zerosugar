#include <boost/lexical_cast.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/database/connection/connection_pool.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/network/rpc/rpc_client.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"
#include "zerosugar/xr/service/database/database_service.h"

namespace zerosugar::xr
{
    class DatabaseServiceApplication final : public AppInstance
    {
    public:
        DatabaseServiceApplication();

        auto GetName() const->std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<db::ConnectionPool> _connectionPool;

        SharedPtrNotNull<RPCClient> _rpcClient;
        SharedPtrNotNull<DatabaseService> _databaseService;
    };

    DatabaseServiceApplication::DatabaseServiceApplication()
        : _executor(std::make_shared<execution::AsioExecutor>(4))
        , _connectionPool(std::make_shared<db::ConnectionPool>(_executor))
        , _rpcClient(std::make_shared<RPCClient>(_executor))
        , _databaseService(std::make_shared<DatabaseService>(_executor, _connectionPool))
    {
        ExecutionContext::PushExecutor(_executor.get());
    }

    auto DatabaseServiceApplication::GetName() const -> std::string_view
    {
        return "database_service_application";
    }

    void DatabaseServiceApplication::OnStartUp(std::span<char*> args)
    {
        ServiceLocator& serviceLocator = GetServiceLocator();

        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);

        auto temp = std::make_shared<LogService>();
        temp->Add(-1, builder.CreateLogger());

        serviceLocator.Add<ILogService>(std::move(temp));
        serviceLocator.Add<RPCClient>(_rpcClient);

        ZEROSUGAR_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] command lines args: {}",
                GetName(), [&]() -> std::string
                {
                    std::ostringstream oss;

                    for (const char* arg : args)
                    {
                        oss << arg << ' ';
                    }

                    std::string result = oss.str();
                    if (!result.empty())
                    {
                        result.pop_back();
                    }

                    return result;
                }()));

        if (args.size() < 8)
        {
            throw std::runtime_error("command line args invalid");
        }

        const std::string address(args[1]);
        const uint16_t port = boost::lexical_cast<uint16_t>(args[2]);

        _executor->Run();

        Future<void> future = _rpcClient->ConnectAsync(address, port);

        auto lastTimePoint = std::chrono::system_clock::now();

        while (future.IsPending())
        {
            const auto now = std::chrono::system_clock::now();

            if (now - lastTimePoint >= std::chrono::seconds(3))
            {
                ZEROSUGAR_LOG_INFO(GetServiceLocator(),
                    fmt::format("[{}] try to connect rpc server...",
                        GetName()));

                lastTimePoint = now;
            }
        }

        const std::string databaseAddress(args[3]);
        const uint16_t databasePort = boost::lexical_cast<uint16_t>(args[4]);
        const std::string databaseUser(args[5]);
        const std::string databasePassword(args[6]);
        const std::string databaseSchema(args[7]);

        const auto endPoint = boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(databaseAddress), databasePort);
        const boost::mysql::handshake_params params(databaseUser, databasePassword, databaseSchema);

        constexpr int64_t connectionCount = 4;
        db::ConnectionPoolOption option(endPoint, params, connectionCount);

        _connectionPool->Initialize(serviceLocator);
        _connectionPool->Start(option);

        _databaseService->Initialize(serviceLocator);

        _rpcClient->RegisterToServer(std::string(_databaseService->GetName())).Get();
    }

    void DatabaseServiceApplication::OnShutdown()
    {

    }
}

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    DatabaseServiceApplication application;

    return application.Run(std::span(argv, argc));
}
