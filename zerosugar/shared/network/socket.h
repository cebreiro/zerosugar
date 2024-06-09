#pragma once
#include <expected>
#include <boost/asio.hpp>
#include "zerosugar/shared/execution/channel/channel.h"
#include "zerosugar/shared/execution/future/future.hpp"
#include "zerosugar/shared/network/io_error.h"
#include "zerosugar/shared/network/buffer/buffer.h"

namespace zerosugar::execution
{
    class AsioStrand;
}

namespace zerosugar
{
    class Buffer;

    class Socket : public std::enable_shared_from_this<Socket>
    {
    public:
        Socket() = delete;

        explicit Socket(SharedPtrNotNull<execution::AsioStrand> strand);

        bool IsOpen() const;

        auto ConnectAsync(std::string address, uint16_t port,
            std::chrono::milliseconds retryInterval = std::chrono::milliseconds(5000),
            std::optional<std::stop_token> token = std::nullopt)
            -> Future<void>;
        void CloseAsync();

        auto SendAsync(Buffer buffer) -> Future<std::expected<int64_t, IOError>>;
        auto ReceiveAsync(Buffer& buffer) -> Future<std::expected<int64_t, IOError>>;

    private:
        std::atomic<bool> _open = false;

        SharedPtrNotNull<execution::AsioStrand> _strand;
        boost::asio::ip::tcp::socket _socket;
    };
}
