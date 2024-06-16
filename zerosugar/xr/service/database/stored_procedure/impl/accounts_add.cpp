#include "accounts_add.h"

namespace zerosugar::xr::db::sp
{
    AccountsAdd::AccountsAdd(zerosugar::db::ConnectionPool::Borrowed& conn, std::string account, std::string password)
        : StoredProcedure(conn)
        , _account(std::move(account))
        , _password(std::move(password))
    {
    }

    auto AccountsAdd::GetSQL() const -> std::string_view
    {
        return "CALL accounts_add(?, ?)";
    }

    auto AccountsAdd::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_account);
        inputs.emplace_back(_password);

        return inputs;
    }

    void AccountsAdd::SetOutput(const boost::mysql::results& result)
    {
        (void)result;
    }
}
