#pragma once
#include <boost/mysql.hpp>
#include <boost/describe.hpp>
#include "zerosugar/sl/database/model/item.h"

namespace zerosugar::sl::db
{
    BOOST_DESCRIBE_STRUCT(zerosugar::sl::db::Item, (), (id, owner_id, data_id, quantity, pos_type, pos_value1, pos_value2, pos_value3))

    class ItemTable
    {
    public:
        ~ItemTable() = default;
        explicit ItemTable(boost::mysql::tcp_ssl_connection& connection);

        void CreateTable();
        void DropTable();

        void Add(const zerosugar::sl::db::Item& item);
        void Remove(const int64_t& id);
        void RemoveByOWNER_ID(const int64_t& owner_id);

        void Assign(const zerosugar::sl::db::Item& item);
        void Update(const zerosugar::sl::db::Item& item);
        void UpdateDifference(const zerosugar::sl::db::Item& oldOne, const zerosugar::sl::db::Item& newOne);

        auto Find(const int64_t& id) -> std::optional<zerosugar::sl::db::Item>;
        auto FindByOWNER_ID(const int64_t& owner_id) -> std::optional<zerosugar::sl::db::Item>;
        auto FindRangeByOWNER_ID(const int64_t& owner_id) -> std::vector<zerosugar::sl::db::Item>;

    private:
        boost::mysql::tcp_ssl_connection& _connection;
    };
}
