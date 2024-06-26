#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::db::sp
{
    class CharactersGet final : public StoredProcedure
    {
    public:
        CharactersGet(zerosugar::db::ConnectionPool::Borrowed& conn, int64_t characterId);

        auto GetResult() const -> const service::DTOCharacter&;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        int64_t _characterId;

        service::DTOCharacter _result;
    };
}
