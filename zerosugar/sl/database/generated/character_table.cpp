#include "character_table.h"

#include <cassert>
#include <sstream>
#include <vector>
#include <boost/container/static_vector.hpp>

#include "zerosugar/sl/database/helper/compare.h"

namespace zerosugar::sl::db
{
    CharacterTable::CharacterTable(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    void CharacterTable::CreateTable()
    {
        constexpr const char* queryString = R"delimiter(
CREATE TABLE IF NOT EXISTS `Character` (
    `id` BIGINT NOT NULL
    , `aid` BIGINT NOT NULL
    , `slot` TINYINT NOT NULL
    , `name` VARCHAR(30) NOT NULL
    , `hair_color` INT NOT NULL
    , `hair` INT NOT NULL
    , `skin_color` INT NOT NULL
    , `face` INT NOT NULL
    , `arms` INT NOT NULL
    , `running` INT NOT NULL
    , `gold` INT NOT NULL
    , `inventory_page` INT NOT NULL
    , `zone` INT NOT NULL
    , `stage` INT NOT NULL
    , `x` FLOAT NOT NULL
    , `y` FLOAT NOT NULL
    , `deleted` TINYINT NOT NULL
    , PRIMARY KEY(`id`)
    , CONSTRAINT `Character_fk_aid` FOREIGN KEY (`aid`)
        REFERENCES `Account` (`id`)
        ON DELETE CASCADE ON UPDATE CASCADE
    , UNIQUE KEY `Character_unique_name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void CharacterTable::DropTable()
    {
        constexpr const char* queryString = R"delimiter(
DROP TABLE IF EXISTS `Character`;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void CharacterTable::Add(const zerosugar::sl::db::Character& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `Character`
    (`id`, `aid`, `slot`, `name`, `hair_color`, `hair`, `skin_color`, `face`, `arms`, `running`, `gold`, `inventory_page`, `zone`, `stage`, `x`, `y`, `deleted`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.id, item.aid, item.slot, item.name, item.hair_color, item.hair, item.skin_color, item.face, item.arms, item.running, item.gold, item.inventory_page, item.zone, item.stage, item.x, item.y, item.deleted);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void CharacterTable::Remove(const int64_t& id)
    {
        constexpr const char* queryString = R"delimiter(
DELETE FROM `Character` WHERE `id` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }
    void CharacterTable::RemoveByAID(const int64_t& aid)
    {
        constexpr const char* queryString = R"delimiter(
DELETE FROM `Character` WHERE `aid` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(aid);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void CharacterTable::Assign(const zerosugar::sl::db::Character& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `Character`
    (`id`, `aid`, `slot`, `name`, `hair_color`, `hair`, `skin_color`, `face`, `arms`, `running`, `gold`, `inventory_page`, `zone`, `stage`, `x`, `y`, `deleted`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON DUPLICATE KEY UPDATE
    `slot` = ?, `name` = ?, `hair_color` = ?, `hair` = ?, `skin_color` = ?, `face` = ?, `arms` = ?, `running` = ?, `gold` = ?, `inventory_page` = ?, `zone` = ?, `stage` = ?, `x` = ?, `y` = ?, `deleted` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.id, item.aid, item.slot, item.name, item.hair_color, item.hair, item.skin_color, item.face, item.arms, item.running, item.gold, item.inventory_page, item.zone, item.stage, item.x, item.y, item.deleted, item.slot, item.name, item.hair_color, item.hair, item.skin_color, item.face, item.arms, item.running, item.gold, item.inventory_page, item.zone, item.stage, item.x, item.y, item.deleted);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }

    void CharacterTable::Update(const zerosugar::sl::db::Character& item)
    {
        constexpr const char* queryString = R"delimiter(
UPDATE `Character`
SET
    `slot` = ?, `name` = ?, `hair_color` = ?, `hair` = ?, `skin_color` = ?, `face` = ?, `arms` = ?, `running` = ?, `gold` = ?, `inventory_page` = ?, `zone` = ?, `stage` = ?, `x` = ?, `y` = ?, `deleted` = ?
WHERE
    id = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.slot, item.name, item.hair_color, item.hair, item.skin_color, item.face, item.arms, item.running, item.gold, item.inventory_page, item.zone, item.stage, item.x, item.y, item.deleted, item.id);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }
    void CharacterTable::UpdateDifference(const zerosugar::sl::db::Character& oldOne, const zerosugar::sl::db::Character& newOne)
    {
        if (oldOne.id != newOne.id)
        {
            assert(false);
            return;
        }

        using field_views_t = boost::container::static_vector<
            boost::mysql::field_view,
            boost::mp11::mp_size<boost::describe::describe_members<
                zerosugar::sl::db::Character,
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
UPDATE `Character`
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

    auto CharacterTable::Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Character>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Character`
WHERE
    `id` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(id);

        boost::mysql::static_results<zerosugar::sl::db::Character> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }
    auto CharacterTable::FindByNAME(const std::string& name) -> std::optional<zerosugar::sl::db::Character>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Character`
WHERE
    `name` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(name);

        boost::mysql::static_results<zerosugar::sl::db::Character> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }
    auto CharacterTable::FindByAID(const int64_t& aid) -> std::optional<zerosugar::sl::db::Character>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Character`
WHERE
    `aid` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(aid);

        boost::mysql::static_results<zerosugar::sl::db::Character> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }
    auto CharacterTable::FindRangeByAID(const int64_t& aid) -> std::vector<zerosugar::sl::db::Character>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `Character`
WHERE
    `aid` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(aid);

        boost::mysql::static_results<zerosugar::sl::db::Character> result;
        _connection.execute(bound, result);

        const auto& front = result.rows();

        std::vector<zerosugar::sl::db::Character> results;
        results.reserve(front.size());

        for (const auto& element : front)
        {
            results.push_back(element);
        }

        return results;
    }

    CharacterStatTable::CharacterStatTable(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    void CharacterStatTable::CreateTable()
    {
        constexpr const char* queryString = R"delimiter(
CREATE TABLE IF NOT EXISTS `CharacterStat` (
    `cid` BIGINT NOT NULL
    , `gender` INT NOT NULL
    , `hp` INT NOT NULL
    , `mp` INT NOT NULL
    , `chr_lv` INT NOT NULL
    , `chr_exp` INT NOT NULL
    , `str` INT NOT NULL
    , `dex` INT NOT NULL
    , `accr` INT NOT NULL
    , `health` INT NOT NULL
    , `intell` INT NOT NULL
    , `wis` INT NOT NULL
    , `will` INT NOT NULL
    , `stat_point` INT NOT NULL
    , `water` INT NOT NULL
    , `fire` INT NOT NULL
    , `lightning` INT NOT NULL
    , UNIQUE KEY `CharacterStat_unique_cid` (`cid`)
    , CONSTRAINT `CharacterStat_fk_cid` FOREIGN KEY (`cid`)
        REFERENCES `Character` (`id`)
        ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void CharacterStatTable::DropTable()
    {
        constexpr const char* queryString = R"delimiter(
DROP TABLE IF EXISTS `CharacterStat`;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void CharacterStatTable::Add(const zerosugar::sl::db::CharacterStat& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `CharacterStat`
    (`cid`, `gender`, `hp`, `mp`, `chr_lv`, `chr_exp`, `str`, `dex`, `accr`, `health`, `intell`, `wis`, `will`, `stat_point`, `water`, `fire`, `lightning`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.cid, item.gender, item.hp, item.mp, item.chr_lv, item.chr_exp, item.str, item.dex, item.accr, item.health, item.intell, item.wis, item.will, item.stat_point, item.water, item.fire, item.lightning);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }


    void CharacterStatTable::Assign(const zerosugar::sl::db::CharacterStat& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `CharacterStat`
    (`cid`, `gender`, `hp`, `mp`, `chr_lv`, `chr_exp`, `str`, `dex`, `accr`, `health`, `intell`, `wis`, `will`, `stat_point`, `water`, `fire`, `lightning`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON DUPLICATE KEY UPDATE
    `gender` = ?, `hp` = ?, `mp` = ?, `chr_lv` = ?, `chr_exp` = ?, `str` = ?, `dex` = ?, `accr` = ?, `health` = ?, `intell` = ?, `wis` = ?, `will` = ?, `stat_point` = ?, `water` = ?, `fire` = ?, `lightning` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.cid, item.gender, item.hp, item.mp, item.chr_lv, item.chr_exp, item.str, item.dex, item.accr, item.health, item.intell, item.wis, item.will, item.stat_point, item.water, item.fire, item.lightning, item.gender, item.hp, item.mp, item.chr_lv, item.chr_exp, item.str, item.dex, item.accr, item.health, item.intell, item.wis, item.will, item.stat_point, item.water, item.fire, item.lightning);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }


    auto CharacterStatTable::FindByCID(const int64_t& cid) -> std::optional<zerosugar::sl::db::CharacterStat>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `CharacterStat`
WHERE
    `cid` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(cid);

        boost::mysql::static_results<zerosugar::sl::db::CharacterStat> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }

    CharacterJobTable::CharacterJobTable(boost::mysql::tcp_ssl_connection& connection)
        : _connection(connection)
    {
    }

    void CharacterJobTable::CreateTable()
    {
        constexpr const char* queryString = R"delimiter(
CREATE TABLE IF NOT EXISTS `CharacterJob` (
    `cid` BIGINT NOT NULL
    , `job1` INT NOT NULL
    , `job1_level` INT NOT NULL
    , `job1_exp` INT NOT NULL
    , `job1_sp` INT NOT NULL
    , `job2` INT NOT NULL
    , `job2_level` INT NOT NULL
    , `job2_exp` INT NOT NULL
    , `job2_sp` INT NOT NULL
    , `job3` INT NOT NULL
    , `job3_level` INT NOT NULL
    , `job3_exp` INT NOT NULL
    , `job3_sp` INT NOT NULL
    , `job_sub` INT NOT NULL
    , `job_sub_level` INT NOT NULL
    , `job_sub_exp` INT NOT NULL
    , `job_sub_sp` INT NOT NULL
    , UNIQUE KEY `CharacterJob_unique_cid` (`cid`)
    , CONSTRAINT `CharacterJob_fk_cid` FOREIGN KEY (`cid`)
        REFERENCES `Character` (`id`)
        ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void CharacterJobTable::DropTable()
    {
        constexpr const char* queryString = R"delimiter(
DROP TABLE IF EXISTS `CharacterJob`;
)delimiter";

        boost::mysql::results result;
        _connection.execute(queryString, result);
    }

    void CharacterJobTable::Add(const zerosugar::sl::db::CharacterJob& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `CharacterJob`
    (`cid`, `job1`, `job1_level`, `job1_exp`, `job1_sp`, `job2`, `job2_level`, `job2_exp`, `job2_sp`, `job3`, `job3_level`, `job3_exp`, `job3_sp`, `job_sub`, `job_sub_level`, `job_sub_exp`, `job_sub_sp`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.cid, item.job1, item.job1_level, item.job1_exp, item.job1_sp, item.job2, item.job2_level, item.job2_exp, item.job2_sp, item.job3, item.job3_level, item.job3_exp, item.job3_sp, item.job_sub, item.job_sub_level, item.job_sub_exp, item.job_sub_sp);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }


    void CharacterJobTable::Assign(const zerosugar::sl::db::CharacterJob& item)
    {
        constexpr const char* queryString = R"delimiter(
INSERT INTO `CharacterJob`
    (`cid`, `job1`, `job1_level`, `job1_exp`, `job1_sp`, `job2`, `job2_level`, `job2_exp`, `job2_sp`, `job3`, `job3_level`, `job3_exp`, `job3_sp`, `job_sub`, `job_sub_level`, `job_sub_exp`, `job_sub_sp`)
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON DUPLICATE KEY UPDATE
    `job1` = ?, `job1_level` = ?, `job1_exp` = ?, `job1_sp` = ?, `job2` = ?, `job2_level` = ?, `job2_exp` = ?, `job2_sp` = ?, `job3` = ?, `job3_level` = ?, `job3_exp` = ?, `job3_sp` = ?, `job_sub` = ?, `job_sub_level` = ?, `job_sub_exp` = ?, `job_sub_sp` = ?;
)delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(item.cid, item.job1, item.job1_level, item.job1_exp, item.job1_sp, item.job2, item.job2_level, item.job2_exp, item.job2_sp, item.job3, item.job3_level, item.job3_exp, item.job3_sp, item.job_sub, item.job_sub_level, item.job_sub_exp, item.job_sub_sp, item.job1, item.job1_level, item.job1_exp, item.job1_sp, item.job2, item.job2_level, item.job2_exp, item.job2_sp, item.job3, item.job3_level, item.job3_exp, item.job3_sp, item.job_sub, item.job_sub_level, item.job_sub_exp, item.job_sub_sp);

        boost::mysql::results result;
        _connection.execute(bound, result);
    }


    auto CharacterJobTable::FindByCID(const int64_t& cid) -> std::optional<zerosugar::sl::db::CharacterJob>
    {
        constexpr const char* queryString = R"delimiter(
SELECT * FROM `CharacterJob`
WHERE
    `cid` = ?;
        )delimiter";
        auto stmt = _connection.prepare_statement(queryString);
        auto bound = stmt.bind(cid);

        boost::mysql::static_results<zerosugar::sl::db::CharacterJob> result;
        _connection.execute(bound, result);

        if (result.rows().empty())
        {
            return std::nullopt;
        }

        return *result.rows().begin();
    }

}
