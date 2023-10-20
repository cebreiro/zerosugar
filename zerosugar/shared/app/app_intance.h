#pragma once
#include <cstdint>
#include <atomic>
#include <boost/system/system_error.hpp>
#include "zerosugar/shared/service/service_locator.h"

namespace zerosugar
{
    namespace execution
    {
        class IExecutor;
        class AsioExecutor;
    }

    class AppInstance
    {
    public:
        AppInstance(const AppInstance& other) = delete;
        AppInstance(AppInstance&& other) noexcept = delete;
        AppInstance& operator=(const AppInstance& other) = delete;
        AppInstance& operator=(AppInstance&& other) noexcept = delete;

        AppInstance();
        explicit AppInstance(int64_t workerCount);
        virtual ~AppInstance();

        auto Run() -> int32_t;
        void Shutdown();

        template <std::derived_from<IService> T>
        auto FindService() -> T*;

        template <std::derived_from<IService> T>
        auto FindService() const -> const T*;

        bool IsRunning() const;

        auto GetServiceLocator() -> ServiceLocator&;
        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetExecutor() -> execution::IExecutor&;
        auto GetExecutor() const -> const execution::IExecutor&;

    protected:
        auto GetAsioExecutor() -> execution::AsioExecutor&;

    private:
        virtual void OnStartUp(ServiceLocator& serviceLocator) = 0;
        virtual void OnShutdown() = 0;
        virtual void OnExit(const std::vector<boost::system::error_code>&);

    private:
        std::atomic<bool> _running = false;
        ServiceLocator _serviceLocator;
        int64_t _workerCount = 1;
        std::shared_ptr<execution::AsioExecutor> _executor;
        std::vector<boost::system::error_code> _errorCodes;
    };

    template <std::derived_from<IService> T>
    auto AppInstance::FindService() -> T*
    {
        return _serviceLocator.Find<T>();
    }

    template <std::derived_from<IService> T>
    auto AppInstance::FindService() const -> const T*
    {
        return _serviceLocator.Find<T>();
    }
}
