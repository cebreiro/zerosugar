#pragma once

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
    };
}
