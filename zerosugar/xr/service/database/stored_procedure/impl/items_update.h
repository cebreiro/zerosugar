#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"

namespace zerosugar::xr::db::sp
{
    class ItemsUpdate final : public StoredProcedure
    {
    public:
        ItemsUpdate(zerosugar::db::ConnectionPool::Borrowed& conn, std::string args);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::string _args;
    };
}
