#include "app_intance.h"

#include <cassert>

#include "zerosugar/shared/app/app.h"
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"

namespace zerosugar
{
    AppInstance::AppInstance()
        : AppInstance(static_cast<int64_t>(std::thread::hardware_concurrency()))
    {
    }

    AppInstance::AppInstance(int64_t workerCount)
        : _workerCount(std::max<int64_t>(1, workerCount))
        , _executor(std::make_shared<execution::AsioExecutor>(_workerCount))
    {
        App::SetInstance(this);
    }

    AppInstance::~AppInstance()
    {
        assert(!IsRunning());
    }

    auto AppInstance::Run() -> int32_t
    {
        _executor->Run();

        this->OnStartUp(_serviceLocator);

        _running.store(true);
        _running.wait(true);

        this->OnShutdown();

        _executor->Stop();
        _executor->Join(&_errorCodes);

        this->OnExit(_errorCodes);

        App::SetInstance(nullptr);

        return _errorCodes.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    void AppInstance::Shutdown()
    {
        _running.store(false);
        _running.notify_one();
    }

    bool AppInstance::IsRunning() const
    {
        return _running.load();
    }

    auto AppInstance::GetServiceLocator() -> ServiceLocator&
    {
        return _serviceLocator;
    }

    auto AppInstance::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    auto AppInstance::GetExecutor() -> execution::IExecutor&
    {
        assert(_executor);
        return *_executor;
    }

    auto AppInstance::GetExecutor() const -> const execution::IExecutor&
    {
        assert(_executor);
        return *_executor;
    }

    auto AppInstance::GetAsioExecutor() -> execution::AsioExecutor&
    {
        assert(_executor);
        return *_executor;
    }

    void AppInstance::OnExit(const std::vector<boost::system::error_code>&)
    {
    }
}
