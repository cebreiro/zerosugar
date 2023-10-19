#include "app_intance.h"

#include "zerosugar/shared/execution/executor/impl/asio_executor.h"

namespace zerosugar
{
    AppInstance::AppInstance()
        : AppInstance(static_cast<int64_t>(std::thread::hardware_concurrency()))
    {
    }

    AppInstance::AppInstance(int64_t workerCount)
        : _workerCount(std::max<int64_t>(1, workerCount))
    {
    }

    AppInstance::~AppInstance()
    {
    }

    auto AppInstance::Run() -> int32_t
    {
        _executor = std::make_shared<execution::AsioExecutor>(_workerCount);
        _executor->Run();

        this->OnStartUp(_serviceLocator);

        _running.wait(true);

        this->OnShutdown();

        _executor->Stop();
        _executor->Join(&_errorCodes);

        this->OnExit(_errorCodes);

        return _errorCodes.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    void AppInstance::Shutdown()
    {
        _running.store(false);
        _running.notify_one();
    }

    auto AppInstance::GetServiceLocator() -> ServiceLocator&
    {
        return _serviceLocator;
    }

    auto AppInstance::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    void AppInstance::OnExit(const std::vector<boost::system::error_code>&)
    {
    }
}
