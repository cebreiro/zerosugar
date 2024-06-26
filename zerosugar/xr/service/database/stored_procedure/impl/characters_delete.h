#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"

namespace zerosugar::xr::db::sp
{
    class CharactersDelete final : public StoredProcedure
    {
    public:
        CharactersDelete(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t characterId);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _characterId;
    };
}
