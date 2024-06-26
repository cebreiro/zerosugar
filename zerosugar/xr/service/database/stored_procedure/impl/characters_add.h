#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::db::sp
{
    class CharactersAdd final : public StoredProcedure
    {
    public:
        CharactersAdd(zerosugar::db::ConnectionPool::Borrowed& conn, const service::DTOCharacterAdd& input);

        auto GetAddedCharacterId() const -> int64_t;

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

    private:
        const service::DTOCharacterAdd& _input;
        int64_t _resultCharacterId = 0;
    };
}
