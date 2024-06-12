#pragma once
#include <boost/mysql.hpp>
#include <boost/describe.hpp>
#include "zerosugar/sl/database/model/character.h"

namespace zerosugar::sl::db
{
    BOOST_DESCRIBE_STRUCT(zerosugar::sl::db::Character, (), (id, aid, slot, name, hair_color, hair, skin_color, face, arms, running, gold, inventory_page, zone, stage, x, y, deleted))

    class CharacterTable
    {
    public:
        ~CharacterTable() = default;
        explicit CharacterTable(boost::mysql::tcp_ssl_connection& connection);

        void CreateTable();
        void DropTable();

        void Add(const zerosugar::sl::db::Character& item);
        void Remove(const int64_t& id);
        void RemoveByAID(const int64_t& aid);

        void Assign(const zerosugar::sl::db::Character& item);
        void Update(const zerosugar::sl::db::Character& item);
        void UpdateDifference(const zerosugar::sl::db::Character& oldOne, const zerosugar::sl::db::Character& newOne);

        auto Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Character>;
        auto FindByNAME(const std::string& name) -> std::optional<zerosugar::sl::db::Character>;
        auto FindByAID(const int64_t& aid) -> std::optional<zerosugar::sl::db::Character>;
        auto FindRangeByAID(const int64_t& aid) -> std::vector<zerosugar::sl::db::Character>;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
    BOOST_DESCRIBE_STRUCT(zerosugar::sl::db::CharacterStat, (), (cid, gender, hp, mp, chr_lv, chr_exp, str, dex, accr, health, intell, wis, will, stat_point, water, fire, lightning))

    class CharacterStatTable
    {
    public:
        ~CharacterStatTable() = default;
        explicit CharacterStatTable(boost::mysql::tcp_ssl_connection& connection);

        void CreateTable();
        void DropTable();

        void Add(const zerosugar::sl::db::CharacterStat& item);

        void Assign(const zerosugar::sl::db::CharacterStat& item);

        auto FindByCID(const int64_t& cid) -> std::optional<zerosugar::sl::db::CharacterStat>;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
    BOOST_DESCRIBE_STRUCT(zerosugar::sl::db::CharacterJob, (), (cid, job1, job1_level, job1_exp, job1_sp, job2, job2_level, job2_exp, job2_sp, job3, job3_level, job3_exp, job3_sp, job_sub, job_sub_level, job_sub_exp, job_sub_sp))

    class CharacterJobTable
    {
    public:
        ~CharacterJobTable() = default;
        explicit CharacterJobTable(boost::mysql::tcp_ssl_connection& connection);

        void CreateTable();
        void DropTable();

        void Add(const zerosugar::sl::db::CharacterJob& item);

        void Assign(const zerosugar::sl::db::CharacterJob& item);

        auto FindByCID(const int64_t& cid) -> std::optional<zerosugar::sl::db::CharacterJob>;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
}
