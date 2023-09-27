#pragma once
#include <cstdint>
#include <vector>
#include <thread>
#include <functional>
#include <boost/asio.hpp>
#include "zerosugar/core/execution/executor_interface.h"

namespace zerosugar::execution
{
    class AsioExecutor : public IExecutor
    {
    public:
        explicit AsioExecutor(int64_t workerCount);
        ~AsioExecutor() override;

        void Run() final;
        void Stop() final;
        void Join(std::vector<boost::system::error_code>* errors) final;

        void Post(const std::function<void()>& function) override;
        void Post(std::move_only_function<void()> function) override;

        auto GetIoContext() -> boost::asio::io_context&;
        auto GetIoContext() const -> const boost::asio::io_context&;

    private:
        int64_t _workerCount = 0;
        std::vector<std::jthread> _workers;
        std::vector<std::future<boost::system::error_code>> _workErrors;
        boost::asio::io_context _ioContext;
        boost::asio::executor_work_guard<decltype(_ioContext)::executor_type> _workGuard;
    };
}
