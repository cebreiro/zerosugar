#pragma once
#include <cstdint>
#include <array>
#include <memory>
#include <random>
#include <boost/asio.hpp>
#include "zerosugar/shared/execution/future/promise.h"

using zerosugar::Promise;

constexpr int64_t clientCount = 50;
constexpr int64_t byteSize = 2000000;

namespace zerosugar::execution
{
    class AsioExecutor;
}

class Client : public std::enable_shared_from_this<Client>
{
public:
    Client() = default;
    Client(zerosugar::execution::AsioExecutor& executor, Promise<boost::system::error_code> promise, uint16_t port, std::mt19937& mt);

    void Start();

    auto GetSendBuffer() const -> const std::array<char, byteSize>&;
    auto GetReceiveBuffer() const -> const std::array<char, byteSize>&;

private:
    void Complete(boost::system::error_code ec);

    void ConnectAsync();
    void OnConnect(const boost::system::error_code& ec);

    void ReceiveAsync();
    void SendAsync();

private:
    std::atomic<bool> _complete = false;
    zerosugar::execution::AsioExecutor* _executor = nullptr;
    boost::asio::strand<boost::asio::io_context::executor_type> _strand;
    Promise<boost::system::error_code> _promise;
    uint16_t _port = 0;
    std::optional<boost::asio::ip::tcp::socket> _socket = std::nullopt;
    std::array<char, byteSize> _bytes = {};
    int64_t _sendSize = 0;
    std::array<char, byteSize> _result = {};
    int64_t _receiveSize = 0;
};
