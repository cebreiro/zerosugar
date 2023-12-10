#include "service_module.h"

#include <boost/mysql.hpp>

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/sl/database/connection/connection_pool.h"
#include "zerosugar/sl/database/connection/connection_pool_option.h"
#include "zerosugar/sl/executable/monolithic_server/config/application_config.h"
#include "zerosugar/sl/service/login/login_service.h"
#include "zerosugar/sl/service/repository/repository_service.h"

namespace zerosugar::sl
{
    void ServiceModule::Initialize(AppInstance& app, ServerConfig& config)
    {
        const DatabaseConfig& dbConfig = config.GetDatabaseConfig();

        _dbExecutor = std::make_shared<execution::AsioExecutor>(dbConfig.GetConnectionCount());
        _dbExecutor->Run();

        auto connectionPool = [&]() -> SharedPtrNotNull<db::ConnectionPool>
            {
                auto endPoint = boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::make_address(dbConfig.GetAddress()), dbConfig.GetPort());
                boost::mysql::handshake_params params(
                    dbConfig.GetUser(),
                    dbConfig.GetPassword(),
                    dbConfig.GetDatabase()
                );

                return std::make_shared<db::ConnectionPool>(_dbExecutor,
                    db::ConnectionPoolOption(std::move(endPoint), params, dbConfig.GetConnectionCount()));
            }();
        connectionPool->Start();
        _connectionPool = std::move(connectionPool);

        const auto concurrency = static_cast<int64_t>(std::thread::hardware_concurrency());
        _executor = std::make_shared<execution::AsioExecutor>(concurrency);
        _executor->Run();

        _repositoryService = std::make_shared<RepositoryService>(app.GetServiceLocator(), *_executor, _connectionPool);
        app.GetServiceLocator().Add<service::IRepositoryService>(_repositoryService);

        _loginService = std::make_shared<LoginService>(app.GetServiceLocator(), *_executor);
        app.GetServiceLocator().Add<service::ILoginService>(_loginService);
    }

    void ServiceModule::Finalize() noexcept
    {
        if (_loginService)
        {
            _loginService->Shutdown();
        }

        if (_repositoryService)
        {
            _repositoryService->Shutdown();
        }

        if (_executor)
        {
            _executor->Stop();
        }

        if (_connectionPool)
        {
            _connectionPool->Stop();
        }

        if (_dbExecutor)
        {
            _dbExecutor->Stop();
        }
    }

    void ServiceModule::GetFinalizeError(std::vector<boost::system::error_code>& errors)
    {
        if (_loginService)
        {
            _loginService->Join(errors);
        }

        if (_repositoryService)
        {
            _repositoryService->Join(errors);
        }

        if (_executor)
        {
            _executor->Join(&errors);
        }

        if (_dbExecutor)
        {
            _dbExecutor->Join(&errors);
        }
    }
}
