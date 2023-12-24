#pragma once
#include <vector>
#include <boost/system/error_code.hpp>

namespace zerosugar
{
    class ServiceLocator;

    class IService
    {
    public:
        IService(const IService& other) = delete;
        IService(IService&& other) noexcept = delete;
        IService& operator=(const IService& other) = delete;
        IService& operator=(IService&& other) noexcept = delete;

        IService() = default;
        virtual ~IService() = default;

        virtual void Initialize([[maybe_unused]] ServiceLocator& dependencyLocator) {}
        virtual void Shutdown() {}
        virtual void Join([[maybe_unused]] std::vector<boost::system::error_code>& errors) {}
    };
}
