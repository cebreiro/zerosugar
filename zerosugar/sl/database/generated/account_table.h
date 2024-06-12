#pragma once
#include <boost/mysql.hpp>
#include <boost/describe.hpp>
#include "zerosugar/sl/database/model/account.h"

namespace zerosugar::sl::db
{
    BOOST_DESCRIBE_STRUCT(zerosugar::sl::db::Account, (), (id, account, password, gm_level, banned, ban_reason))

    class AccountTable
    {
    public:
        ~AccountTable() = default;
        explicit AccountTable(boost::mysql::tcp_ssl_connection& connection);

        void CreateTable();
        void DropTable();

        void Add(const zerosugar::sl::db::Account& item);
        void Remove(const int64_t& id);

        void Assign(const zerosugar::sl::db::Account& item);
        void Update(const zerosugar::sl::db::Account& item);
        void UpdateDifference(const zerosugar::sl::db::Account& oldOne, const zerosugar::sl::db::Account& newOne);

        auto Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Account>;
        auto FindByACCOUNT(const std::string& account) -> std::optional<zerosugar::sl::db::Account>;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
}
