#include "account_table.h"

#include <cassert>
#include <sstream>
#include <vector>
#include <boost/container/static_vector.hpp>

#include "zerosugar/sl/database/helper/compare.h"

namespace zerosugar::sl::db
{
    AccountTable::AccountTable(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    void AccountTable::CreateTable()
    {
        constexpr const char* queryString = R"delimiter(
CREATE TABLE IF NOT EXISTS `Account` (
    `id` BIGINT NOT NULL
    , `account` VARCHAR(30) NOT NULL
    , `password` VARCHAR(30) NOT NULL
    , `gm_level` TINYINT NOT NULL
    , `banned` TINYINT NOT NULL
    , `ban_reason` VARCHAR(1024)
    , PRIMARY KEY(`id`)
    , UNIQUE KEY `Account_unique_account` (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void AccountTable::DropTable()
    {
        constexpr const char* queryString = R"delimiter(
DROP TABLE IF EXISTS `Account`;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void AccountTable::Add(const zerosugar::sl::db::Account& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `Account`
    (`id`, `account`, `password`, `gm_level`, `banned`, `ban_reason`)
VALUES
    (?, ?, ?, ?, ?, ?);
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.id, item.account, item.password, item.gm_level, item.banned, item.ban_reason);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void AccountTable::Remove(const int64_t& id)
    {
        constexpr const char* queryString = R"delimiter(
DELETE FROM `Account` WHERE `id` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void AccountTable::Assign(const zerosugar::sl::db::Account& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `Account`
    (`id`, `account`, `password`, `gm_level`, `banned`, `ban_reason`)
VALUES
    (?, ?, ?, ?, ?, ?)
ON DUPLICATE KEY UPDATE
    `account` = ?, `password` = ?, `gm_level` = ?, `banned` = ?, `ban_reason` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.id, item.account, item.password, item.gm_level, item.banned, item.ban_reason, item.account, item.password, item.gm_level, item.banned, item.ban_reason);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void AccountTable::Update(const zerosugar::sl::db::Account& item)
    {
        constexpr const char* queryString = R"delimiter(
UPDATE `Account`
SET
    `account` = ?, `password` = ?, `gm_level` = ?, `banned` = ?, `ban_reason` = ?
WHERE
    id = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.account, item.password, item.gm_level, item.banned, item.ban_reason, item.id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }
    void AccountTable::UpdateDifference(const zerosugar::sl::db::Account& oldOne, const zerosugar::sl::db::Account& newOne)
    {
        if (oldOne.id != newOne.id)
        {
            assert(false);
            return;
        }

        using field_views_t = boost::container::static_vector<
            boost::mysql::field_view,
            boost::mp11::mp_size<boost::describe::describe_members<
                zerosugar::sl::db::Account,
                boost::describe::mod_any_access>>::value>;
        std::ostringstream oss;
        field_views_t views;

        if (!FillDifference(oldOne, newOne, oss, views))
        {
            return;
        }
        
        views.emplace_back(newOne.id);
        
        std::string difference = oss.str();
        assert(!difference.empty());
        
        difference.pop_back();
        constexpr const char* queryString = R"delimiter(
UPDATE `Account`
SET
    {}
WHERE
    id = ?;
)delimiter";

        const std::string dynamicQuery = std::format(queryString, difference);
        auto stmt = _connection.prepare_statement(dynamicQuery);
        auto bound = stmt.bind(views.begin(), views.end());

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    auto AccountTable::Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Account>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Account`
WHERE
    `id` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(id);

        boost::mysql::static_results<zerosugar::sl::db::Account> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }
    auto AccountTable::FindByACCOUNT(const std::string& account) -> std::optional<zerosugar::sl::db::Account>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Account`
WHERE
    `account` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(account);

        boost::mysql::static_results<zerosugar::sl::db::Account> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }

}
