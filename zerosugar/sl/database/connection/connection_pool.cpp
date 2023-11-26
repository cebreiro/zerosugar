#include "connection_pool.h"

#include "zerosugar/shared/execution/executor/strand.h"
#include "zerosugar/shared/execution/executor/executor_coroutine_traits.h"
#include "zerosugar/shared/execution/executor/static_thread_pool.h"
#include "zerosugar/shared/execution/executor/operation/schedule.h"
#include "zerosugar/shared/execution/future/future_coroutine_traits.h"
#include "zerosugar/shared/execution/future/operation/delay.h"

namespace zerosugar::sl::db
{
    ConnectionPool::ConnectionPool(SharedPtrNotNull<execution::IExecutor> executor,
        ConnectionPoolOption option)
        : _executor(std::move(executor))
        , _option(std::move(option))
    {
    }

    void ConnectionPool::Start()
    {
        for (int32_t i = 0; i < _option.GetConnectionCount(); ++i)
        {
            Connect();
        }

        constexpr auto delay = std::chrono::seconds(60);
        _pingStopSource = Schedule(*_executor, [self = shared_from_this()]()
            {
                self->Ping();
            }, delay, delay);
    }

    void ConnectionPool::Stop()
    {
        _pingStopSource.request_stop();
    }

    auto ConnectionPool::Lend() -> Future<BorrowedConnection>
    {
        while (true)
        {
            if (std::unique_ptr<boost::mysql::tcp_ssl_connection> connection = TryPopConnection(); connection)
            {
                boost::mysql::tcp_ssl_connection& reference = *connection;
                SetConnectionBorrowed(std::move(connection));

                co_return BorrowedConnection(reference, shared_from_this());
            }
            else
            {
                co_await static_cast<execution::IExecutor&>(StaticThreadPool::GetInstance());
                co_await Delay(std::chrono::milliseconds(1));
            }
        }
    }

    void ConnectionPool::TakeBack(boost::mysql::tcp_ssl_connection& connection)
    {
        const size_t key = reinterpret_cast<size_t>(&connection);

        decltype(_borrowedConnections)::accessor accessor;
        if (_borrowedConnections.find(accessor, key))
        {
            _idleConnections.push(std::move(accessor->second));
            _borrowedConnections.erase(accessor);
        }
        else
        {
            assert(false);
        }
    }

    void ConnectionPool::Connect()
    {
        boost::asio::ssl::context sslContext(boost::asio::ssl::context::tls_client);
        auto connection = std::make_unique<boost::mysql::tcp_ssl_connection>(_ioContext, sslContext);

        connection->connect(_option.GetEndPoint(), _option.GetHandShakeParams());

        _idleConnections.push(std::move(connection));
    }

    auto ConnectionPool::TryPopConnection() -> std::unique_ptr<boost::mysql::tcp_ssl_connection>
    {
        std::unique_ptr<boost::mysql::tcp_ssl_connection> connection;
        (void)_idleConnections.try_pop(connection);

        return connection;
    }

    void ConnectionPool::SetConnectionBorrowed(std::unique_ptr<boost::mysql::tcp_ssl_connection> connection)
    {
        const size_t key = reinterpret_cast<size_t>(connection.get());

        decltype(_borrowedConnections)::accessor accessor;
        if (_borrowedConnections.insert(accessor, key))
        {
            accessor->second = std::move(connection);
        }
        else
        {
            assert(false);
        }
    }

    auto ConnectionPool::Ping() -> Future<void>
    {
        auto holder = this->shared_from_this();

        co_await static_cast<execution::IExecutor&>(StaticThreadPool::GetInstance());

        std::vector<std::unique_ptr<boost::mysql::tcp_ssl_connection>> connections;
        std::unique_ptr<boost::mysql::tcp_ssl_connection> context;

        while (_idleConnections.try_pop(context))
        {
            connections.push_back(std::move(context));
        }

        for (std::unique_ptr<boost::mysql::tcp_ssl_connection>& connection : connections)
        {
            boost::mysql::error_code ec;
            boost::mysql::diagnostics diagnostics;
            connection->ping(ec, diagnostics);

            if (ec)
            {
                // TODO: log
                // TODO: try reconnect until success

                continue;
            }
            else
            {
                _idleConnections.push(std::move(connection));
            }
        }
    }
}
