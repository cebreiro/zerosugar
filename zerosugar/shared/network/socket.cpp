#include "socket.h"

#include "zerosugar/shared/network/io_error.h"
#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/impl/asio_strand.h"

namespace zerosugar
{
    Socket::Socket(SharedPtrNotNull<execution::AsioStrand> strand)
        : _strand(std::move(strand))
        , _socket(_strand->GetImpl())
    {
    }

    bool Socket::IsOpen() const
    {
        return _open.load();
    }

    auto Socket::ConnectAsync(std::string address, uint16_t port,
        std::chrono::milliseconds retryInterval, std::optional<std::stop_token> token)
        -> Future<void>
    {
        [[maybe_unused]]
        const auto guard = shared_from_this();

        const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(address), port);

        co_await *_strand;
        assert(ExecutionContext::GetExecutor() == _strand.get());

        while (true)
        {
            Promise<bool> promise;
            Future<bool> future = promise.GetFuture();

            _socket.async_connect(endpoint,
                [this, p = std::move(promise)](const boost::system::error_code& ec) mutable
                {
                    p.Set(ec ? false : true);
                });

            if (co_await future)
            {
                assert(ExecutionContext::GetExecutor() == _strand.get());

                _open.store(true);

                co_return;
            }

            if (token && token->stop_requested())
            {
                co_return;
            }

            co_await Delay(retryInterval);
            assert(ExecutionContext::GetExecutor() == _strand.get());
        }
    }

    void Socket::CloseAsync()
    {
        bool expected = true;

        if (_open.compare_exchange_strong(expected, false))
        {
            Post(*_strand, [self = shared_from_this()]()
                {
                    self->_socket.close();
                });
        }
    }

    auto Socket::SendAsync(Buffer buffer) -> Future<std::expected<int64_t, IOError>>
    {
        if (ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        assert(ExecutionContext::GetExecutor() == _strand.get());

        Promise<std::expected<int64_t, IOError>> promise;
        Future<std::expected<int64_t, IOError>> future = promise.GetFuture();

        {
            boost::container::small_vector<boost::asio::const_buffer, 4> constBuffers;

            for (const buffer::Fragment& fragment : buffer.GetFragmentContainer())
            {
                constBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
            }

            async_write(_socket, constBuffers,
                bind_executor(_strand->GetImpl(),
                    [p = std::move(promise), buffer = std::move(buffer)](const boost::system::error_code& ec, size_t bytes) mutable
                    {
                        if (ec)
                        {
                            p.Set(std::unexpected<IOError>(ec));
                        }
                        else
                        {
                            p.Set(static_cast<int64_t>(bytes));
                        }
                    }));
        }

        co_return co_await future;
    }

    auto Socket::ReceiveAsync(Buffer& buffer) -> Future<std::expected<int64_t, IOError>>
    {
        if (ExecutionContext::IsEqualTo(*_strand))
        {
            co_await *_strand;
        }

        assert(ExecutionContext::GetExecutor() == _strand.get());

        Promise<std::expected<int64_t, IOError>> promise;
        Future<std::expected<int64_t, IOError>> future = promise.GetFuture();

        {
            boost::container::small_vector<boost::asio::mutable_buffer, 4> mutableBuffers;

            for (buffer::Fragment& fragment : buffer.GetFragmentContainer())
            {
                mutableBuffers.emplace_back(fragment.GetData(), fragment.GetSize());
            }

            _socket.async_receive(mutableBuffers,
                bind_executor(_strand->GetImpl(),
                    [p = std::move(promise)](const boost::system::error_code& ec, size_t bytes) mutable
                    {
                        if (ec)
                        {
                            p.Set(std::unexpected<IOError>(ec));
                        }
                        else
                        {
                            p.Set(bytes);
                        }
                    }));
        }

        co_return co_await future;
    }
}
