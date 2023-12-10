#pragma once
#include <vector>
#include <boost/system/error_code.hpp>

namespace zerosugar
{
    class IService
    {
    public:
        IService(const IService& other) = delete;
        IService(IService&& other) noexcept = delete;
        IService& operator=(const IService& other) = delete;
        IService& operator=(IService&& other) noexcept = delete;

        IService() = default;
        virtual ~IService() = default;

        virtual void Shutdown() {}
        virtual void Join(std::vector<boost::system::error_code>&) {}
    };
}
