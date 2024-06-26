#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::db::sp
{
    class AccountsGet final : public StoredProcedure
    {
    public:
        AccountsGet(zerosugar::db::ConnectionPool::Borrowed& conn, std::string account);

        auto GetResultAccount() const -> const std::optional<service::DTOAccount>&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        std::string _account;
        std::optional<service::DTOAccount> _result;
    };
}
