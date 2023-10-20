#include "app.h"

#include "zerosugar/shared/app/app_intance.h"
#include "zerosugar/shared/service/service_locator.h"
#include "zerosugar/shared/execution/executor/static_thread_pool.h"

namespace zerosugar
{
    std::atomic<AppInstance*> App::_instance = nullptr;
    thread_local AppInstance* App::_localInstance = nullptr;

    void App::SetInstance(AppInstance* instance)
    {
        _instance.store(instance);
    }

    void App::Shutdown()
    {
        if (AppInstance* instance = LoadInstance(); instance != nullptr)
        {
            instance->Shutdown();
        }
    }

    auto App::GetServiceLocator() -> ServiceLocator&
    {
        if (AppInstance* instance = LoadInstance(); instance != nullptr)
        {
            return instance->GetServiceLocator();
        }

        static ServiceLocator nullObject;
        return nullObject;
    }

    auto App::GetExecutor() -> execution::IExecutor&
    {
        if (AppInstance* instance = LoadInstance(); instance != nullptr)
        {
            return instance->GetExecutor();
        }

        return StaticThreadPool::GetInstance();
    }

    auto App::LoadInstance() -> AppInstance*
    {
        if (_localInstance == nullptr)
        {
            _localInstance = _instance.load();
        }

        return _localInstance;
    }
}
