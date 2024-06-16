#include "accounts_get.h"

namespace zerosugar::xr::db::sp
{
    AccountsGet::AccountsGet(zerosugar::db::ConnectionPool::Borrowed& conn, std::string account)
        : StoredProcedure(conn)
        , _account(std::move(account))
    {
    }

    auto AccountsGet::GetResultAccount() const -> const std::optional<service::DTOAccount>&
    {
        return _result;
    }

    auto AccountsGet::GetSQL() const -> std::string_view
    {
        return "CALL accounts_get(?)";
    }

    auto AccountsGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_account);

        return inputs;
    }

    void AccountsGet::SetOutput(const boost::mysql::results& result)
    {
        if (result.rows().empty())
        {
            return;
        }

        const boost::mysql::row_view& front = result.rows().front();

        size_t index = 0;
        service::DTOAccount& dto = _result.emplace();

        dto.accountId = front.at(index++).as_int64();
        dto.account = front.at(index++).as_string();
        dto.password = front.at(index++).as_string();
        dto.gmLevel = static_cast<bool>(front.at(index++).as_int64());
        dto.banned = static_cast<bool>(front.at(index++).as_int64());
        dto.deleted = static_cast<bool>(front.at(index++).as_int64());
    }
}
