#pragma once
#include <boost/mysql.hpp>
#include <boost/describe.hpp>
#include "zerosugar/sl/database/model/skill.h"

namespace zerosugar::sl::db
{
    BOOST_DESCRIBE_STRUCT(zerosugar::sl::db::Skill, (), (id, owner_id, job_id, skill_id, level, cooldown, pos_type, pos_value1, pos_value2, pos_value3))

    class SkillTable
    {
    public:
        ~SkillTable() = default;
        explicit SkillTable(boost::mysql::tcp_ssl_connection& connection);

        void CreateTable();
        void DropTable();

        void Add(const zerosugar::sl::db::Skill& item);
        void Remove(const int64_t& id);
        void RemoveByOWNER_ID(const int64_t& owner_id);

        void Assign(const zerosugar::sl::db::Skill& item);
        void Update(const zerosugar::sl::db::Skill& item);
        void UpdateDifference(const zerosugar::sl::db::Skill& oldOne, const zerosugar::sl::db::Skill& newOne);

        auto Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Skill>;
        auto FindByOWNER_ID(const int64_t& owner_id) -> std::optional<zerosugar::sl::db::Skill>;
        auto FindRangeByOWNER_ID(const int64_t& owner_id) -> std::vector<zerosugar::sl::db::Skill>;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
}
