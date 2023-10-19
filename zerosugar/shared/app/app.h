#pragma once
#include <atomic>

namespace zerosugar
{
    class AppInstance;
    class ServiceLocator;

    class App
    {
    public:
        App() = delete;

        static void Set(AppInstance& instance);
        static void Shutdown();

        static auto GetServiceLocator() -> ServiceLocator&;

    private:
        static auto LoadInstance() -> AppInstance*;

    private:
        static std::atomic<AppInstance*> _instance;
        static thread_local AppInstance* _localInstance;
    };
}
