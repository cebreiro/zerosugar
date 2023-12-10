#pragma once
#include <boost/mysql/error_code.hpp>

namespace zerosugar
{
    class AppInstance;
}

namespace zerosugar::sl
{
    class ServerConfig;

    class IModule
    {
    public:
        IModule() = default;
        IModule(const IModule& other) = delete;
        IModule(IModule&& other) noexcept = delete;
        IModule& operator=(const IModule& other) = delete;
        IModule& operator=(IModule&& other) noexcept = delete;

    public:
        virtual ~IModule() = default;

        virtual void Initialize(AppInstance& app, ServerConfig& config) = 0;
        virtual void Finalize() noexcept = 0;
        virtual void GetFinalizeError(std::vector<boost::system::error_code>& errors) = 0;
    };
}
