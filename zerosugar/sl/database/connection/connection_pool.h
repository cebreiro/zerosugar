#pragma once
#include <atomic>
#include <boost/mysql.hpp>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <oneapi/tbb/concurrent_queue.h>
#include "zerosugar/shared/type/borrowed.h"
#include "zerosugar/shared/execution/future/future.h"
#include "zerosugar/sl/database/connection/connection_pool_option.h"

namespace zerosugar
{
    namespace execution
    {
        class IExecutor;
    }

    class Strand;
}

namespace zerosugar::sl::db
{
    namespace detail
    {
        class BorrowedConnectionOwner
        {
        public:
            virtual ~BorrowedConnectionOwner() = default;
            virtual void TakeBack(boost::mysql::tcp_ssl_connection& connection) = 0;
            virtual auto Lend() -> Future<Borrowed<boost::mysql::tcp_ssl_connection, BorrowedConnectionOwner>> = 0;
        };
    }

    class ConnectionPool
        : public detail::BorrowedConnectionOwner
        , public std::enable_shared_from_this<ConnectionPool>
    {
    public:
        using BorrowedConnection = Borrowed<boost::mysql::tcp_ssl_connection, BorrowedConnectionOwner>;

    public:
        ConnectionPool(SharedPtrNotNull<execution::IExecutor> executor, ConnectionPoolOption option);

        void Start();
        void Stop();

        auto Lend() -> Future<BorrowedConnection> override;
        void TakeBack(boost::mysql::tcp_ssl_connection& connection) override;

    private:
        struct Context;

        void Connect();
        auto TryPopConnection() -> std::unique_ptr<boost::mysql::tcp_ssl_connection>;
        void SetConnectionBorrowed(std::unique_ptr<boost::mysql::tcp_ssl_connection> connection);

        auto Ping() -> Future<void>;

    private:
        boost::asio::io_context _ioContext;
        SharedPtrNotNull<execution::IExecutor> _executor;

        ConnectionPoolOption _option;

        tbb::concurrent_queue<std::shared_ptr<Context>> _contexts;

        tbb::concurrent_queue<std::unique_ptr<boost::mysql::tcp_ssl_connection>> _idleConnections;
        tbb::concurrent_hash_map<size_t, std::unique_ptr<boost::mysql::tcp_ssl_connection>> _borrowedConnections;
        std::stop_source _pingStopSource;
    };
}
