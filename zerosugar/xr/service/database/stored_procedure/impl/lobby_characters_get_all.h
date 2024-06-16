#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::db::sp
{
    class LobbyCharactersGetAll final
        : public StoredProcedure
    {
    public:
        LobbyCharactersGetAll(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t accountId);

        inline auto GetResult() const;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _accountId = 0;
        std::unordered_map<int64_t, service::DTOLobbyCharacter> _results;
    };

    auto LobbyCharactersGetAll::GetResult() const
    {
        return _results | std::views::values;
    }
}
