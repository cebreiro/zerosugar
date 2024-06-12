#include "skill_table.h"

#include <cassert>
#include <sstream>
#include <vector>
#include <boost/container/static_vector.hpp>

#include "zerosugar/sl/database/helper/compare.h"

namespace zerosugar::sl::db
{
    SkillTable::SkillTable(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    void SkillTable::CreateTable()
    {
        constexpr const char* queryString = R"delimiter(
CREATE TABLE IF NOT EXISTS `Skill` (
    `id` BIGINT NOT NULL
    , `owner_id` BIGINT NOT NULL
    , `job_id` INT NOT NULL
    , `skill_id` INT NOT NULL
    , `level` INT NOT NULL
    , `cooldown` BIGINT NOT NULL
    , `pos_type` TINYINT NOT NULL
    , `pos_value1` TINYINT NOT NULL
    , `pos_value2` TINYINT NOT NULL
    , `pos_value3` TINYINT NOT NULL
    , PRIMARY KEY(`id`)
    , KEY `Skill_idx_owner_id` (`owner_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void SkillTable::DropTable()
    {
        constexpr const char* queryString = R"delimiter(
DROP TABLE IF EXISTS `Skill`;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void SkillTable::Add(const zerosugar::sl::db::Skill& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `Skill`
    (`id`, `owner_id`, `job_id`, `skill_id`, `level`, `cooldown`, `pos_type`, `pos_value1`, `pos_value2`, `pos_value3`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.id, item.owner_id, item.job_id, item.skill_id, item.level, item.cooldown, item.pos_type, item.pos_value1, item.pos_value2, item.pos_value3);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void SkillTable::Remove(const int64_t& id)
    {
        constexpr const char* queryString = R"delimiter(
DELETE FROM `Skill` WHERE `id` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }
    void SkillTable::RemoveByOWNER_ID(const int64_t& owner_id)
    {
        constexpr const char* queryString = R"delimiter(
DELETE FROM `Skill` WHERE `owner_id` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(owner_id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void SkillTable::Assign(const zerosugar::sl::db::Skill& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `Skill`
    (`id`, `owner_id`, `job_id`, `skill_id`, `level`, `cooldown`, `pos_type`, `pos_value1`, `pos_value2`, `pos_value3`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON DUPLICATE KEY UPDATE
    `owner_id` = ?, `job_id` = ?, `skill_id` = ?, `level` = ?, `cooldown` = ?, `pos_type` = ?, `pos_value1` = ?, `pos_value2` = ?, `pos_value3` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.id, item.owner_id, item.job_id, item.skill_id, item.level, item.cooldown, item.pos_type, item.pos_value1, item.pos_value2, item.pos_value3, item.owner_id, item.job_id, item.skill_id, item.level, item.cooldown, item.pos_type, item.pos_value1, item.pos_value2, item.pos_value3);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void SkillTable::Update(const zerosugar::sl::db::Skill& item)
    {
        constexpr const char* queryString = R"delimiter(
UPDATE `Skill`
SET
    `owner_id` = ?, `job_id` = ?, `skill_id` = ?, `level` = ?, `cooldown` = ?, `pos_type` = ?, `pos_value1` = ?, `pos_value2` = ?, `pos_value3` = ?
WHERE
    id = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.owner_id, item.job_id, item.skill_id, item.level, item.cooldown, item.pos_type, item.pos_value1, item.pos_value2, item.pos_value3, item.id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }
    void SkillTable::UpdateDifference(const zerosugar::sl::db::Skill& oldOne, const zerosugar::sl::db::Skill& newOne)
    {
        if (oldOne.id != newOne.id)
        {
            assert(false);
            return;
        }

        using field_views_t = boost::container::static_vector<
            boost::mysql::field_view,
            boost::mp11::mp_size<boost::describe::describe_members<
                zerosugar::sl::db::Skill,
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
UPDATE `Skill`
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

    auto SkillTable::Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Skill>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Skill`
WHERE
    `id` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(id);

        boost::mysql::static_results<zerosugar::sl::db::Skill> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }
    auto SkillTable::FindByOWNER_ID(const int64_t& owner_id) -> std::optional<zerosugar::sl::db::Skill>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Skill`
WHERE
    `owner_id` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(owner_id);

        boost::mysql::static_results<zerosugar::sl::db::Skill> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }
    auto SkillTable::FindRangeByOWNER_ID(const int64_t& owner_id) -> std::vector<zerosugar::sl::db::Skill>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Skill`
WHERE
    `owner_id` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(owner_id);

        boost::mysql::static_results<zerosugar::sl::db::Skill> result;
        _connection.execute(bound, result);

        const auto& front = result.rows();

        std::vector<zerosugar::sl::db::Skill> results;
        results.reserve(front.size());

        for (const auto& element : front)
        {
            results.push_back(element);
        }

        return results;
    }

}
