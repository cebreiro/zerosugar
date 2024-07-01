#include "items_update.h"

namespace zerosugar::xr::db::sp
{
    ItemsUpdate::ItemsUpdate(zerosugar::db::ConnectionPool::Borrowed& conn, std::string args)
        : StoredProcedure(conn)
        , _args(std::move(args))
    {
    }

    auto ItemsUpdate::GetSQL() const -> std::string_view
    {
        return "CALL items_update(?)";
    }

    auto ItemsUpdate::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> result;
        result.emplace_back(_args);

        return result;
    }

    void ItemsUpdate::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}
