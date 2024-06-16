#pragma once
#include <boost/mysql.hpp>
#include <boost/container/small_vector.hpp>
#include "zerosugar/shared/database/database_error.h"
#include "zerosugar/shared/database/connection/connection_pool.h"

namespace zerosugar::xr::db
{
    class StoredProcedure
    {
    public:
        StoredProcedure() = delete;
        StoredProcedure(const StoredProcedure& other) = delete;
        StoredProcedure(StoredProcedure&& other) noexcept = delete;
        StoredProcedure& operator=(const StoredProcedure& other) = delete;
        StoredProcedure& operator=(StoredProcedure&& other) noexcept = delete;

        explicit StoredProcedure(zerosugar::db::ConnectionPool::Borrowed& conn);
        virtual ~StoredProcedure() = default;

        auto ExecuteAsync() noexcept -> Future<DatabaseError>;

    private:
        virtual auto GetSQL() const -> std::string_view = 0;
        virtual auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> = 0;
        virtual void SetOutput(const boost::mysql::results& result) = 0;

    private:
        zerosugar::db::ConnectionPool::Borrowed& _conn;
        boost::mysql::diagnostics _dbDiagnostics = {};
        boost::mysql::results _executeResult = {};
    };
}
