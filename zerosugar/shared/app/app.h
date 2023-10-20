#pragma once
#include <atomic>

namespace zerosugar
{
    class AppInstance;
    class ServiceLocator;

    namespace execution
    {
        class IExecutor;
    }

    class App
    {
    public:
        App() = delete;

        static void SetInstance(AppInstance* instance);
        static void Shutdown();

        static auto GetServiceLocator() -> ServiceLocator&;
        static auto GetExecutor() -> execution::IExecutor&;

    private:
        static auto LoadInstance() -> AppInstance*;

    private:
        static std::atomic<AppInstance*> _instance;
        static thread_local AppInstance* _localInstance;
    };
}
