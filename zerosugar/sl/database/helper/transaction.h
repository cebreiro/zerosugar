#pragma once
#include <boost/mysql.hpp>

namespace zerosugar::sl::db
{
    class Transaction
    {
    public:
        Transaction(const Transaction& other) = delete;
        Transaction(Transaction&& other) noexcept = delete;
        Transaction& operator=(const Transaction& other) = delete;
        Transaction& operator=(Transaction&& other) noexcept = delete;

    public:
        Transaction(boost::mysql::tcp_ssl_connection& connection);

        void Start();
        void Commit();
        void Rollback();

    private:
        boost::mysql::tcp_ssl_connection& _connection;
        bool _started = false;
        bool _handled = false;
    };
}
