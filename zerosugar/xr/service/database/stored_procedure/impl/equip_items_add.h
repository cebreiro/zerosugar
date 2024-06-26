#pragma once
#include "zerosugar/xr/service/database/stored_procedure/stored_procedure.h"
#include "zerosugar/xr/service/model/generated/data_transfer_object_message.h"

namespace zerosugar::xr::db::sp
{
    class EquipItemsAdd final : public StoredProcedure
    {
    public:
        EquipItemsAdd(zerosugar::db::ConnectionPool::Borrowed& conn,
            int64_t characterId, const std::vector<service::DTOEquipItem>& inputs);

    private:
        auto GetSQL() const -> std::string_view override;
        auto GetInput() const -> boost::container::small_vector<boost::mysql::field, 16> override;
        void SetOutput(const boost::mysql::results& result) override;

        int64_t _cid = 0;
        std::string _json;
    };
}
