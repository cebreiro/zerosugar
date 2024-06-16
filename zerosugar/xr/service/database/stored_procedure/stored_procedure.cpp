#include "stored_procedure.h"

namespace zerosugar::xr::db
{
    StoredProcedure::StoredProcedure(zerosugar::db::ConnectionPool::Borrowed& conn)
        : _conn(conn)
    {
    }

    auto StoredProcedure::ExecuteAsync() noexcept -> Future<DatabaseError>
    {
        try
        {
            std::string_view sql = this->GetSQL();

            boost::mysql::statement stmt = _conn->prepare_statement(sql);

            Promise<boost::mysql::error_code> promise;
            Future<boost::mysql::error_code> future = promise.GetFuture();

            const boost::container::small_vector<boost::mysql::field, 16>& input = this->GetInput();

            _conn->async_execute(stmt.bind(input.begin(), input.end()), _executeResult, _dbDiagnostics,
                [p = std::move(promise)](boost::mysql::error_code ec) mutable
                {
                    p.Set(std::move(ec));
                });

            const boost::system::error_code ec = co_await future;
            throw_on_error(ec, _dbDiagnostics);

            this->SetOutput(_executeResult);

        }
        catch (const boost::mysql::error_with_diagnostics& e)
        {
            co_return DatabaseError(e);
        }
        catch (const std::exception& e)
        {
            co_return DatabaseError(e);
        }
        catch (...)
        {
            co_return DatabaseError();
        }

        co_return {};
    }
}
