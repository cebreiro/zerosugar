#include "client.h"

#include <fmt/format.h>
#include "zerosugar/shared/execution/executor/impl/asio_executor.h"
#include "zerosugar/shared/execution/future/operation/delay.h"

Client::Client(zerosugar::execution::AsioExecutor& executor, Promise<boost::system::error_code> promise, uint16_t port,
    std::mt19937& mt)
    : _executor(&executor)
    , _strand(make_strand(executor.GetIoContext()))
    , _promise(std::move(promise))
    , _port(port)
{
    std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());

    for (int64_t i = 0; i < byteSize; ++i)
    {
        _bytes[i] = static_cast<char>(dist(mt) % std::numeric_limits<uint8_t>::max());
    }
}

void Client::Start()
{
    assert(!_socket.has_value());
    _socket = boost::asio::ip::tcp::socket(_executor->GetIoContext());

    ConnectAsync();
}

auto Client::GetSendBuffer() const -> const std::array<char, byteSize>&
{
    return _bytes;
}


auto Client::GetReceiveBuffer() const -> const std::array<char, byteSize>&
{
    return _result;
}

void Client::Complete(boost::system::error_code ec)
{
    bool expected = false;
    if (_complete.compare_exchange_strong(expected, true))
    {
        dispatch(_strand, [self = shared_from_this(), ec]() mutable
            {
                self->_socket->close();
                self->_socket.reset();

                self->_promise.Set(std::move(ec));
            });
    }
}

void Client::ConnectAsync()
{
    using namespace boost::asio::ip;

    _socket->async_connect(tcp::endpoint(make_address("127.0.0.1"), _port),
        [self = shared_from_this()](const boost::system::error_code& ec)
        {
            self->OnConnect(ec);
        });
}

void Client::OnConnect(const boost::system::error_code& ec)
{
    if (ec)
    {
        std::cout << fmt::format("fail to connect({}, {})... retry after 3s\n", ec.value(), ec.message());
        zerosugar::Delay(std::chrono::seconds(3)).Then(
            *_executor, [self =shared_from_this()]()
            {
                self->ConnectAsync();
            });

        return;
    }

    ReceiveAsync();
    SendAsync();
}

void Client::ReceiveAsync()
{
    assert(byteSize - _receiveSize > 0);

    _socket->async_read_some(boost::asio::buffer(_result.data() + _receiveSize, byteSize - _receiveSize),
        bind_executor(_strand, [self = shared_from_this(), this](const boost::system::error_code& ec, size_t n)
            {
                if (ec)
                {
                    Complete(ec);

                    return;
                }

                _receiveSize += static_cast<int64_t>(n);
                if (_receiveSize < byteSize)
                {
                    ReceiveAsync();
                }
                else if (_receiveSize == byteSize)
                {
                    Complete(boost::system::error_code());
                }
                else
                {
                    Complete(boost::asio::error::broken_pipe);
                }
            }));
}

void Client::SendAsync()
{
    assert(std::ssize(_bytes) - _sendSize > 0);

    async_write(*_socket, boost::asio::buffer(_bytes.data() + _sendSize, std::ssize(_bytes) - _sendSize),
        bind_executor(_strand, [self = shared_from_this(), this](const boost::system::error_code& ec, size_t n)
            {
                if (ec)
                {
                    Complete(ec);

                    return;
                }

                _sendSize += static_cast<int64_t>(n);
                if (_sendSize == byteSize)
                {
                    return;
                }
                else
                {
                    // is it possible?
                    assert(false);
                    SendAsync();
                }
            }));
}
