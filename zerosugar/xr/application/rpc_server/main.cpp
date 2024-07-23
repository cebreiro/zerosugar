#include <boost/lexical_cast.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/xr/network/rpc/rpc_server.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger.h"
#include "zerosugar/shared/log/spdlog/spdlog_logger_builder.h"

namespace zerosugar::xr
{
    class RPCServerApplication final : public AppInstance
    {
    public:
        RPCServerApplication();

        auto GetName() const -> std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        SharedPtrNotNull<execution::AsioExecutor> _executor;
        SharedPtrNotNull<RPCServer> _rpcServer;
    };

    RPCServerApplication::RPCServerApplication()
        : _executor(std::make_shared<execution::AsioExecutor>(4))
        , _rpcServer(std::make_shared<RPCServer>(_executor))
    {
    }

    auto RPCServerApplication::GetName() const -> std::string_view
    {
        return "rpc_server_application";
    }

    void RPCServerApplication::OnStartUp(std::span<char*> args)
    {
        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Info).SetAsync(false);

        auto temp = std::make_shared<LogService>();
        temp->Add(-1, builder.CreateLogger());

        GetServiceLocator().Add<ILogService>(std::move(temp));

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

        if (args.size() < 2)
        {
            throw std::runtime_error("command line args[1] -> rpc server port is null");
        }

        const uint16_t port = boost::lexical_cast<uint16_t>(args[1]);

        _executor->Run();
        _rpcServer->StartUp(port);
    }

    void RPCServerApplication::OnShutdown()
    {
        _rpcServer->Shutdown();
    }
}

int main(int argc, char* argv[])
{
    using namespace zerosugar::xr;

    (void)argc;
    (void)argv;

    RPCServerApplication application;

    std::string args1 = "";
    std::string args2 = "8888";

    std::vector<char*> asd{ args1.data(), args2.data() };

    //return application.Run(std::span(argv, argc));
    return application.Run(asd);
}
