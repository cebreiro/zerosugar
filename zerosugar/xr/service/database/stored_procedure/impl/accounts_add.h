#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"

namespace zerosugar::xr::db::sp
{
    class AccountsAdd final : public StoredProcedure
    {
    public:
        AccountsAdd(zerosugar::db::ConnectionPool::Borrowed& conn, std::string account, std::string password);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::string _account;
        std::string _password;
    };
}
