#pragma once
#include <boost/mysql/error_code.hpp>

namespace zerosugar
{
    class AppInstance;
}

namespace zerosugar::sl
{
    class AppConfig;

    class IAssembler
    {
    public:
        IAssembler() = default;
        IAssembler(const IAssembler& other) = delete;
        IAssembler(IAssembler&& other) noexcept = delete;
        IAssembler& operator=(const IAssembler& other) = delete;
        IAssembler& operator=(IAssembler&& other) noexcept = delete;

    public:
        virtual ~IAssembler() = default;

        virtual void Initialize(AppInstance& app, AppConfig& config) = 0;
        virtual void Finalize() noexcept = 0;
        virtual void GetFinalizeError(std::vector<boost::system::error_code>& errors) = 0;
    };
}
