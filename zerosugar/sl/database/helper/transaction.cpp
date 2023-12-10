#include "transaction.h"

namespace zerosugar::sl::db
{
    Transaction::Transaction(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    void Transaction::Start()
    {
        const bool prev = std::exchange(_started, true);
        if (prev)
        {
            assert(false);
            return;
        }

        try
        {
            boost::mysql::results result;
            _connection.execute("START TRANSACTION", result);
        }
        catch (...)
        {
            _started = false;
            throw;
        }
    }

    void Transaction::Commit()
    {
        if (!_started)
        {
            assert(false);
            return;
        }

        const bool prev = std::exchange(_handled, true);
        if (prev)
        {
            assert(false);
            return;
        }

        try
        {
            boost::mysql::results result;
            _connection.execute("COMMIT", result);
        }
        catch (...)
        {
            _handled = false;
            throw;
        }
    }

    void Transaction::Rollback()
    {
        if (!_started)
        {
            assert(false);
            return;
        }

        const bool prev = std::exchange(_handled, true);
        if (prev)
        {
            assert(false);
            return;
        }

        try
        {
            boost::mysql::results result;
            _connection.execute("ROLLBACK", result);
        }
        catch (...)
        {
            _handled = false;
            throw;
        }
    }
}
